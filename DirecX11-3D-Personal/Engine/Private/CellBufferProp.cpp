#include "CellBufferProp.h"

Engine::CCellBufferProp::CCellBufferProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CVIBuffer(pDevice, pContext)
{
}

HRESULT Engine::CCellBufferProp::Initialize(const Vector3& vPoint0, const Vector3& vPoint1, const Vector3& vPoint2)
{
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXPOS);
	m_iNumVertices = 3;
	m_vecVertexPos.resize(m_iNumVertices);

	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = 4;
	m_vecIndex.resize(m_iNumIndices);
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

#pragma region VERTEX_BUFFER
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	m_vecVertexPos[0] = pVertices[0].vPosition = vPoint0;
	m_vecVertexPos[1] = pVertices[1].vPosition = vPoint1;
	m_vecVertexPos[2] = pVertices[2].vPosition = vPoint2;

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
	m_vecIndex[1] = pIndices[1] = 1;
	m_vecIndex[2] = pIndices[2] = 2;
	m_vecIndex[3] = pIndices[3] = 0;

	m_InitialData.pSysMem = pIndices;

	HRESULT resultIB = __super::Create_Buffer(m_pIB.GetAddressOf());

	Safe_Delete_Array(pIndices);

	if (FAILED(resultIB)) return E_FAIL;
#pragma endregion

	return S_OK;
}

shared_ptr<CCellBufferProp> Engine::CCellBufferProp::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const Vector3& vPoint0, const Vector3& vPoint1, const Vector3& vPoint2)
{
	shared_ptr<CCellBufferProp> pInstance = make_shared<CCellBufferProp>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vPoint0, vPoint1, vPoint2)))
	{
		MSG_BOX("Failed to Created : CCellBufferProp");
		pInstance.reset();
	}

	return pInstance;
}
