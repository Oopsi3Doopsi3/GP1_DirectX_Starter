#pragma once

class Effect;

struct Vertex
{
	dae::Vector3 position;
	dae::ColorRGB color;
};

class Mesh
{
public:
	Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	~Mesh();

	void Render(ID3D11DeviceContext* pDeviceContext) const;

private:
	ID3D11Device* m_pDevice{ nullptr };
	Effect* m_pEffect{ nullptr };
	ID3DX11EffectTechnique* m_pTechnique{ nullptr };

	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	uint32_t m_NumIndices{};

	ID3D11Buffer* m_pVertexBuffer{ nullptr };
	ID3D11Buffer* m_pIndexBuffer{ nullptr };
	ID3D11InputLayout* m_pInputLayout{ nullptr };
};

