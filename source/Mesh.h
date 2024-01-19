#pragma once

#include "Texture.h"

class Effect;

struct Vertex
{
	dae::Vector3 position;
	dae::ColorRGB color;
	dae::Vector2 uv;
	dae::Vector3 normal;
	dae::Vector3 tangent;
};

class Mesh
{
public:
	Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	~Mesh();

	void Render(ID3D11DeviceContext* pDeviceContext) const;

	void Update(const dae::Matrix projectionMatrix, const dae::Matrix& inverseViewMatrix);

	void RotateX(const float angle);
	void RotateY(const float angle);
	void RotateZ(const float angle);

	void SetDiffuseMap(const dae::Texture* pDiffuseTexture) const;
	void SetNormalMap(const dae::Texture* pNormalTexture) const;
	void SetSpecularMap(const dae::Texture* pSpecularTexture) const;
	void SetGlossinessMap(const dae::Texture* pGlossinessTexture) const;

	void SetPass(const int passIdx) {m_Pass = passIdx;};
private:
	//Effect
	Effect* m_pEffect{ nullptr };
	ID3DX11EffectTechnique* m_pTechnique{ nullptr };

	//Render
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	uint32_t m_NumIndices{};

	ID3D11Buffer* m_pVertexBuffer{ nullptr };
	ID3D11Buffer* m_pIndexBuffer{ nullptr };
	ID3D11InputLayout* m_pInputLayout{ nullptr };

	//Update
	dae::Matrix m_TranslationMatrix{ dae::Vector3::UnitX,dae::Vector3::UnitY, dae::Vector3::UnitZ, dae::Vector3::Zero };
	dae::Matrix m_RotationMatrix{ dae::Vector3::UnitX,dae::Vector3::UnitY, dae::Vector3::UnitZ, dae::Vector3::Zero };
	dae::Matrix m_ScaleMatrix{ dae::Vector3::UnitX,dae::Vector3::UnitY, dae::Vector3::UnitZ, dae::Vector3::Zero };

	//Variables
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{ nullptr };
	ID3DX11EffectMatrixVariable* m_pWorldMatrixVariable{ nullptr };
	ID3DX11EffectMatrixVariable* m_pViewInverseVariable{ nullptr };

	//Texture Maps
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{ nullptr };

	UINT m_Pass{ 0 };

	void InitializeVariables();
	void InitializeTextures();

	void SetMatWorldViewProj(const dae::Matrix& matrix) const;
	void SetWorldMatrixVariable(const dae::Matrix& matrix) const;
	void SetViewInverseVariable(const dae::Matrix& matrix) const;
};

