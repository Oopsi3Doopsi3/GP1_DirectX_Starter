#include "pch.h"
#include "Effect.h"
//#include "Texture.h"

using namespace dae;

Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
	: m_pEffect{ LoadEffect(pDevice, assetFile) }
{
	//Technique
	m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
	if (!m_pTechnique->IsValid())
		std::wcout << L"Technique not valid\n";

	//World
	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
		std::wcout << L"m_pMatWorldViewProjVariable not valid!\n";

	//Texture
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
		std::wcout << L"m_pDiffuseMapVariable is not valid!\n";
}

Effect::~Effect()
{
	if (m_pEffect) m_pEffect->Release();
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}

ID3DX11EffectTechnique* Effect::GetTechnique() const
{
	return m_pTechnique;
}

//World
void Effect::SetMatWorldViewProj(const Matrix& matrix) const
{
	m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void Effect::SetWorldMatrixVariable(const Matrix& matrix) const
{

}

void Effect::SetViewInverseVariable(const Matrix& matrix) const
{

}

//Texture
void Effect::SetDiffuseMap(const dae::Texture* pDiffuseTexture)
{
	if (pDiffuseTexture)
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetSRV());
}

void Effect::SetUseNormalMap(const bool useNormalMap) const
{

}