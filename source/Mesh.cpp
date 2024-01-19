#include "pch.h"
#include "Mesh.h"
#include "Effect.h"
#include <cassert>

Mesh::Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices):
	m_Vertices{vertices},
	m_Indices{indices}
{
	m_pEffect = new Effect(pDevice, L"Resources/PosCol3D.fx");
	m_pTechnique = m_pEffect->GetTechnique();
	
	InitializeVariables();
	InitializeTextures();
	
	//Create Vertex Layout
	static constexpr uint32_t numElements{ 5 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "TEXCOORD";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 24;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "NORMAL";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[4].SemanticName = "TANGENT";
	vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[4].AlignedByteOffset = 44;
	vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Create Input Layout
	D3DX11_PASS_DESC passDesc{};
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result))
		assert(false);

	//Create Vertex Buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices.data();

	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return;

	//Create Index Buffer
	m_NumIndices = static_cast<uint32_t>(indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
		return;
}

void Mesh::InitializeVariables()
{
	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
		std::wcout << L"m_pMatWorldViewProjVariable not valid!\n";

	m_pWorldMatrixVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pWorldMatrixVariable->IsValid())
		std::wcout << L"m_pWorldMatrixVariable not valid!\n";

	m_pViewInverseVariable = m_pEffect->GetVariableByName("gViewInverseMatrix")->AsMatrix();
	if (!m_pViewInverseVariable->IsValid())
		std::wcout << L"m_pViewInverseVariable not valid!\n";
}

void Mesh::InitializeTextures()
{
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
		std::wcout << L"m_pDiffuseMapVariable is not valid!\n";

	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
		std::wcout << L"m_pNormalMapVariable is not valid!\n";

	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
		std::wcout << L"m_pSpecularMapVariable is not valid!\n";

	m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMapVariable->IsValid())
		std::wcout << L"m_pGlossinessMapVariable is not valid!\n";
}

Mesh::~Mesh()
{
	delete m_pEffect;

	if (m_pVertexBuffer) m_pVertexBuffer->Release();
	if (m_pIndexBuffer) m_pIndexBuffer->Release();
	if (m_pInputLayout) m_pInputLayout->Release();

	if (m_pMatWorldViewProjVariable) m_pMatWorldViewProjVariable->Release();
	if (m_pWorldMatrixVariable) m_pWorldMatrixVariable->Release();
	if (m_pViewInverseVariable) m_pViewInverseVariable->Release();

	if (m_pDiffuseMapVariable) m_pDiffuseMapVariable->Release();
	if (m_pNormalMapVariable) m_pNormalMapVariable->Release();
	if (m_pSpecularMapVariable) m_pSpecularMapVariable->Release();
	if (m_pGlossinessMapVariable) m_pGlossinessMapVariable->Release();
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext) const
{
	//1. Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//2. Set Input Layout
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	//3. Set VertexBuffer
	constexpr UINT stride = sizeof(Vertex);
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//4. Set IndexBUffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//5. Draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pTechnique->GetDesc(&techDesc);
	if (m_Pass < techDesc.Passes)
	{
		m_pTechnique->GetPassByIndex(m_Pass)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}

void Mesh::Update(const dae::Matrix projectionMatrix, const dae::Matrix& inverseViewMatrix)
{
	dae::Matrix worldMatrix = m_ScaleMatrix * m_RotationMatrix * m_TranslationMatrix;
	SetMatWorldViewProj(worldMatrix * inverseViewMatrix * projectionMatrix);
	SetWorldMatrixVariable(worldMatrix);
	SetViewInverseVariable(inverseViewMatrix);
}

void Mesh::RotateX(const float angle)
{
	m_RotationMatrix = dae::Matrix::CreateRotationX(angle) * m_RotationMatrix;
}

void Mesh::RotateY(const float angle)
{
	m_RotationMatrix = dae::Matrix::CreateRotationY(angle) * m_RotationMatrix;
}

void Mesh::RotateZ(const float angle)
{
	m_RotationMatrix = dae::Matrix::CreateRotationZ(angle) * m_RotationMatrix;
}

void Mesh::SetMatWorldViewProj(const dae::Matrix& matrix) const
{
	m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<const float*>(&matrix));
}
void Mesh::SetWorldMatrixVariable(const dae::Matrix& matrix) const
{
	m_pWorldMatrixVariable->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void Mesh::SetViewInverseVariable(const dae::Matrix& matrix) const
{
	m_pViewInverseVariable->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void Mesh::SetDiffuseMap(const dae::Texture* pDiffuseTexture) const
{
	if (pDiffuseTexture)
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetSRV());
}

void Mesh::SetNormalMap(const dae::Texture* pNormalTexture) const
{
	if (pNormalTexture)
		m_pNormalMapVariable->SetResource(pNormalTexture->GetSRV());
}

void Mesh::SetSpecularMap(const dae::Texture* pSpecularTexture) const
{
	if (pSpecularTexture)
		m_pSpecularMapVariable->SetResource(pSpecularTexture->GetSRV());
}

void Mesh::SetGlossinessMap(const dae::Texture* pGlossinessTexture) const
{
	if (pGlossinessTexture)
		m_pGlossinessMapVariable->SetResource(pGlossinessTexture->GetSRV());
}