#pragma once
#include "Texture.h"

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	virtual ~Effect();

	Effect(const Effect& other) = delete;
	Effect& operator=(const Effect& other) = delete;
	Effect(Effect&& other) = delete;
	Effect& operator=(Effect&& other) = delete;

	ID3DX11EffectTechnique* GetTechnique() const;

	//World
	void SetMatWorldViewProj(const dae::Matrix& matrix) const;
	virtual void SetWorldMatrixVariable(const dae::Matrix& matrix) const;
	virtual void SetViewInverseVariable(const dae::Matrix& matrix) const;

	//Texture
	void SetDiffuseMap(const dae::Texture* pDiffuseTexture);

	virtual void SetUseNormalMap(const bool useNormalMap) const;
protected:
	ID3DX11Effect* m_pEffect{ nullptr };
	ID3DX11EffectTechnique* m_pTechnique{ nullptr };

	//World
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{ nullptr };

	//Texture
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
};