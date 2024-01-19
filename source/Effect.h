#pragma once
#include "Texture.h"
//class Texture;

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

	ID3DX11Effect* GetEffect() const;
	ID3DX11EffectTechnique* GetTechnique() const;
	ID3DX11EffectMatrixVariable* GetWorldViewProjMat() const;

	//Texture Maps
	ID3DX11EffectShaderResourceVariable* GetDiffuseMap() const;
	ID3DX11EffectShaderResourceVariable* GetNormalMap() const;
	ID3DX11EffectShaderResourceVariable* GetSpecularMap() const;
	ID3DX11EffectShaderResourceVariable* GetGlossinessMap() const;
	
private:
	ID3DX11Effect* m_pEffect{ nullptr };
	ID3DX11EffectTechnique* m_pTechnique{ nullptr };
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{ nullptr };

	//Texture Maps
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{ nullptr };
};