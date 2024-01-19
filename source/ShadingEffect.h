#pragma once
#include "Effect.h"
#include "Texture.h"

class ShadingEffect final : public Effect
{
public:
	ShadingEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	virtual ~ShadingEffect();

	ShadingEffect(const ShadingEffect& other) = delete;
	ShadingEffect& operator=(const ShadingEffect& other) = delete;
	ShadingEffect(ShadingEffect&& other) = delete;
	ShadingEffect& operator=(ShadingEffect&& other) = delete;

	//World
	virtual void SetWorldMatrixVariable(const dae::Matrix& matrix) const override;
	virtual void SetViewInverseVariable(const dae::Matrix& matrix) const override;

	//Texture
	void SetNormalMap(const dae::Texture* pNormalTexture);
	void SetSpecularMap(const dae::Texture* pSpecularTexture);
	void SetGlossinessMap(const dae::Texture* pGlossinessTexture);

	//Variable
	virtual void SetUseNormalMap(const bool useNormalMap) const override;

private:
	//World
	ID3DX11EffectMatrixVariable* m_pWorldMatrixVariable{ nullptr };
	ID3DX11EffectMatrixVariable* m_pViewInverseVariable{ nullptr };

	//Texture
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{ nullptr };

	//Variable
	ID3DX11EffectScalarVariable* m_pUseNormalMap{ nullptr };
};

