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
	Mesh(ID3D11Device* pDevice, const std::string& filename, Effect* pEffect);
	~Mesh();

	void Render(ID3D11DeviceContext* pDeviceContext) const;

	void Update(const dae::Matrix projectionMatrix, const dae::Matrix& inverseViewMatrix);

	void RotateX(const float angle);
	void RotateY(const float angle);
	void RotateZ(const float angle);

	void SetPass(const int passIdx) {m_Pass = passIdx;};
	void SetUseNormalMap(const bool useNormalMap);
private:
	//Effect
	Effect* m_pEffect{ nullptr };
	ID3DX11EffectTechnique* m_pTechnique{ nullptr };

	//Render
	uint32_t m_NumIndices{};

	ID3D11Buffer* m_pVertexBuffer{ nullptr };
	ID3D11Buffer* m_pIndexBuffer{ nullptr };
	ID3D11InputLayout* m_pInputLayout{ nullptr };

	//Update
	dae::Matrix m_TranslationMatrix{ dae::Vector3::UnitX,dae::Vector3::UnitY, dae::Vector3::UnitZ, dae::Vector3::Zero };
	dae::Matrix m_RotationMatrix{ dae::Vector3::UnitX,dae::Vector3::UnitY, dae::Vector3::UnitZ, dae::Vector3::Zero };
	dae::Matrix m_ScaleMatrix{ dae::Vector3::UnitX,dae::Vector3::UnitY, dae::Vector3::UnitZ, dae::Vector3::Zero };

	UINT m_Pass{ 0 };
};

