#include "pch.h"
#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>

#include <iostream>

namespace dae
{
	Texture::Texture(SDL_Surface* pSurface, ID3D11Device* pDevice) :
		m_pSurface{ pSurface },
		m_pSurfacePixels{ (uint32_t*)pSurface->pixels }
	{
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = pSurface->w;
		desc.Height = pSurface->h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);

		HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &m_pResource);
		if (FAILED(hr))
		{
			std::cout << "CreateTexture2D failed: " << hr << std::endl;
			return;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDEsc{};
		SRVDEsc.Format = format;
		SRVDEsc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDEsc.Texture2D.MipLevels = 1;

		hr = pDevice->CreateShaderResourceView(m_pResource, &SRVDEsc, &m_pSRV);

		if (m_pSurface)
		{
			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}
	}

	Texture::~Texture()
	{
		if (m_pSurface)
		{
			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}
		if (m_pResource) m_pResource->Release();
		if (m_pSRV) m_pSRV->Release();
	}

	Texture* Texture::LoadFromFile(const std::string& path, ID3D11Device* pDevice)
	{
		//TODO
		//Load SDL_Surface using IMG_LOAD
		SDL_Surface* tex_surf = IMG_Load(path.c_str());

		if (!tex_surf)
		{
			std::cout << "Unable to load texture from: " << path.c_str() << std::endl;
			return 0;
		}
		//Create & Return a new Texture Object (using SDL_Surface)

		return new Texture(tex_surf, pDevice);
	}

	ColorRGB Texture::Sample(Vector2& uv) const
	{
		//TODO
		//Sample the correct texel for the given uv

		//Convert from [0,1] to [0, width/height]
		const int u = static_cast<int>(uv.x * m_pSurface->w);
		const int v = static_cast<int>(uv.y * m_pSurface->h);

		//Convert to single array
		const int surfacePixel = m_pSurfacePixels[u + v * m_pSurface->w];

		Uint8 r{};
		Uint8 g{};
		Uint8 b{};
		SDL_GetRGB(surfacePixel, m_pSurface->format, &r, &g, &b);

		return { r / 255.f,
				g / 255.f,
				b / 255.f };
	}

	ID3D11ShaderResourceView* Texture::GetSRV() const
	{
		return m_pSRV;
	}
}