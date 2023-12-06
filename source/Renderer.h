#pragma once

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;

	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		//DIRECTX
		HRESULT InitializeDirectX();
		
		//1. Create Device & DeviceContext
		ID3D11Device* m_pDevice{};
		ID3D11DeviceContext* m_pDeviceContext{};

		//2. Create Swapchain
		IDXGISwapChain* m_pSwapChain{};

		//3. Create DepthStencil (DS) & DepthStencilView (DSV)
		ID3D11Texture2D* m_pDepthStencilBuffer{};
		ID3D11DepthStencilView* m_pDepthStencilView{};

		//4. Create RenderTarget (RT) & RenderTargetView (RTV)
		ID3D11Resource* m_pRenderTargetBuffer{};
		ID3D11RenderTargetView* m_pRenderTargetView{};
	};
}