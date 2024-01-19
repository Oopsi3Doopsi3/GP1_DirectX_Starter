#include "pch.h"
#include "ShadingEffect.h"

ShadingEffect::ShadingEffect(ID3D11Device* pDevice, const std::wstring& asssetFile)
	:Effect(pDevice, asssetFile)
{
	//World
	m_pWorldMatrixVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pWorldMatrixVariable->IsValid())
		std::wcout << L"m_pWorldMatrixVariable not valid!\n";

	m_pViewInverseVariable = m_pEffect->GetVariableByName("gViewInverseMatrix")->AsMatrix();
	if (!m_pViewInverseVariable->IsValid())
		std::wcout << L"m_pViewInverseVariable not valid!\n";

	//Texture
	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
		std::wcout << L"m_pNormalMapVariable is not valid!\n";

	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
		std::wcout << L"m_pSpecularMapVariable is not valid!\n";

	m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMapVariable->IsValid())
		std::wcout << L"m_pGlossinessMapVariable is not valid!\n";

	//Variable
	m_pUseNormalMap = m_pEffect->GetVariableByName("gUseNormalMap")->AsScalar();
	if (!m_pUseNormalMap->IsValid())
		std::wcout << L"m_pUseNormalMap not valid!\n";
}

ShadingEffect::~ShadingEffect()
{

}

//World
void ShadingEffect::SetWorldMatrixVariable(const dae::Matrix& matrix) const
{
	m_pWorldMatrixVariable->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ShadingEffect::SetViewInverseVariable(const dae::Matrix& matrix) const
{
	m_pViewInverseVariable->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

//Texture
void ShadingEffect::SetNormalMap(const dae::Texture* pNormalTexture)
{
	if (pNormalTexture)
		m_pNormalMapVariable->SetResource(pNormalTexture->GetSRV());
}

void ShadingEffect::SetSpecularMap(const dae::Texture* pSpecularTexture)
{
	if (pSpecularTexture)
		m_pSpecularMapVariable->SetResource(pSpecularTexture->GetSRV());
}

void ShadingEffect::SetGlossinessMap(const dae::Texture* pGlossinessTexture)
{
	if (pGlossinessTexture)
		m_pGlossinessMapVariable->SetResource(pGlossinessTexture->GetSRV());
}

//Variable
void ShadingEffect::SetUseNormalMap(const bool useNormalMap) const
{
	m_pUseNormalMap->SetBool(useNormalMap);
}