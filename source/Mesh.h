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

	void Render(ID3D11DeviceContext* pDeviceContext, const float* pData) const;

	void SetDiffuseMap(const dae::Texture* pDiffuseTexture) const;
	void SetPass(const int passIdx);
private:
	Effect* m_pEffect{ nullptr };
	ID3DX11EffectTechnique* m_pTechnique{ nullptr };
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{ nullptr };

	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	uint32_t m_NumIndices{};

	ID3D11Buffer* m_pVertexBuffer{ nullptr };
	ID3D11Buffer* m_pIndexBuffer{ nullptr };
	ID3D11InputLayout* m_pInputLayout{ nullptr };

	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };
	UINT m_Pass{ 0 };
};

