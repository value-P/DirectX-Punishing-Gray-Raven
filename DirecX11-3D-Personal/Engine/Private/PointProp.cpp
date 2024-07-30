#include "PointProp.h"

Engine::CPointProp::CPointProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CVIBuffer(pDevice, pContext)
{
}

HRESULT Engine::CPointProp::Initialize()
{
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;
	m_vecVertexPos.resize(m_iNumVertices);

	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = 1;
	m_vecIndex.resize(m_iNumIndices);
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);
	
	m_vecVertexPos[0] = pVertices[0].vPosition = Vector3(0.f, 0.f, 0.f);
	pVertices[0].vPSize = Vector2(1.f, 1.f);

	m_InitialData.pSysMem = pVertices;

	HRESULT resultVB = __super::Create_Buffer(m_pVB.GetAddressOf());

	Safe_Delete_Array(pVertices);

	if (FAILED(resultVB)) return E_FAIL;
#pragma endregion

#pragma region INDEX_BUFFER
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);
	
	m_vecIndex[0] = pIndices[0] = 0;

	m_InitialData.pSysMem = pIndices;

	HRESULT resultIB = __super::Create_Buffer(m_pIB.GetAddressOf());

	Safe_Delete_Array(pIndices);

	if (FAILED(resultIB)) return E_FAIL;
#pragma endregion

	return S_OK;
}

shared_ptr<CPointProp> Engine::CPointProp::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CPointProp> pInstance = make_shared<CPointProp>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPointProp");
		pInstance.reset();
	}

	return pInstance;
}
