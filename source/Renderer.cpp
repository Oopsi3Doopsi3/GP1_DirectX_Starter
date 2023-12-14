#include "pch.h"
#include "Renderer.h"

//#include "Mesh.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
		m_Camera.Initialize(static_cast<float>(m_Width) / static_cast<float>(m_Height), 45.f, { 0.f,0.f,-10.f });

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		//Create some data for our mesh
		//std::vector<Vertex> vertices{
		//	{{ 0.f,  3.f, 2.f}, {1.f, 0.f, 0.f}},
		//	{{ 3.f, -3.f, 2.f}, {0.f, 0.f, 1.f}},
		//	{{-3.f, -3.f, 2.f}, {0.f, 1.f, 0.f}}
		//};
		//
		//std::vector<uint32_t> indices{ 0,1,2 };
		//
		//m_pMesh = new Mesh(m_pDevice, vertices, indices);

		std::vector<Vertex> verticesQuad{
			{{-3.f, 3.f, -2.f}, {}, {0.f, 0.f}},
			{{3.f, 3.f, -2.f}, {}, {1.f, 0.f}},
			{{-3.f, -3.f, -2.f}, {}, {0.f, 1.f}},
			{{3.f, -3.f , -2.f}, {}, {1.f, 1.f}}
		};
		
		std::vector<uint32_t> indicesQuad{ 0,1,2,2,1,3 };
		
		m_pMesh = new Mesh(m_pDevice, verticesQuad, indicesQuad);
		m_pDiffuseTexture = Texture::LoadFromFile("Resources/uv_grid_2.png", m_pDevice);
		m_pMesh->SetDiffuseMap(m_pDiffuseTexture);
	}

	Renderer::~Renderer()
	{
		if(m_pRenderTargetView) m_pRenderTargetView->Release();
		if(m_pRenderTargetBuffer) m_pRenderTargetBuffer->Release();
		if(m_pDepthStencilView) m_pDepthStencilView->Release();
		if(m_pDepthStencilBuffer) m_pDepthStencilBuffer->Release();
		if(m_pSwapChain) m_pSwapChain->Release();
		
		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}

		if(m_pDevice) m_pDevice->Release();

		delete m_pMesh;
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_Camera.Update(pTimer);
	}


	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;

		//1. CLEAR RTV & DSV
		constexpr float color[4] = { 0.f,0.f,0.3f,1.f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		//2. SET PIPELINE + INVOKE DRAW CALLS (= RENDER)
		Matrix viewProjectionMatrix = m_Camera.invViewMatrix * m_Camera.projectionMatrix;

		m_pMesh->Render(m_pDeviceContext, reinterpret_cast<float*>(&viewProjectionMatrix));

		//3. PRESENT BACKBUFFER (SWAP)
		m_pSwapChain->Present(0, 0);
	}

	HRESULT Renderer::InitializeDirectX()
	{
		//1. Create Device & DeviceContext
		//=====
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // (DEBUG) || defined(_DEBUG)

		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel,
			1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);

		if (FAILED(result))
			return result;

		//Create DXGI Factory
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result))
			return result;

		//2. Create Swapchain
		//=====
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		//Get the handle (HWND) from the SDL backbuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_GetVersion(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		//Create SwapChain
		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);

		pDxgiFactory->Release(); //Release here? IDK?

		if (FAILED (result))
			return result;

		//3. Create DepthStencil (DS) & DepthStencilView (DSV)
		//Resource
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		//View
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
			return result;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
			return result;

		//4. Create RenderTarget (RT) & RenderTargetView (RTV)
		//=====

		//Resource
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
			return result;

		//View
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
			return result;

		//5. Bind RTV & DSV to Output Merger Stage
		//=====
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		//6. Set Viewport
		//=====
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);


		return result;
	}

	void Renderer::CycleSamplerState()
	{
		switch (m_SamplerState)
		{
		case dae::Renderer::SamplerState::Point:
			m_pMesh->SetPass(1);
			m_SamplerState = SamplerState::Linear;
			std::cout << "Linear\n";
			break;
		case dae::Renderer::SamplerState::Linear:
			m_pMesh->SetPass(2);
			m_SamplerState = SamplerState::Anisotropic;
			std::cout << "Anisotropic\n";
			break;
		case dae::Renderer::SamplerState::Anisotropic:
			m_pMesh->SetPass(0);
			m_SamplerState = SamplerState::Point;
			std::cout << "Point\n";
			break;
		default:
			break;
		}
	}
}
