#pragma once
#include "pch.h"

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();

	ID3DX11Effect* GetEffect();
private:
	ID3DX11Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pTechnique{};

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
};