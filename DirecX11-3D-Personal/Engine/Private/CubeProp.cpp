#include "CubeProp.h"

CCubeProp::CCubeProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CVIBuffer(pDevice, pContext)
{
}

HRESULT CCubeProp::Initialize()
{
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXCUBE);
	m_iNumVertices = 8;
	m_vecVertexPos.resize(m_iNumVertices);

	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = 36;
	m_vecIndex.resize(m_iNumIndices);
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXCUBE* pVertices = new VTXCUBE[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXCUBE) * m_iNumVertices);

	m_vecVertexPos[0] = pVertices[0].vPosition = Vector3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTexCoord = pVertices[0].vPosition;

	m_vecVertexPos[1] = pVertices[1].vPosition = Vector3(0.5f, 0.5f, -0.5f);
	pVertices[1].vTexCoord = pVertices[1].vPosition;

	m_vecVertexPos[2] = pVertices[2].vPosition = Vector3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTexCoord = pVertices[2].vPosition;

	m_vecVertexPos[3] = pVertices[3].vPosition = Vector3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vTexCoord = pVertices[3].vPosition;

	m_vecVertexPos[4] = pVertices[4].vPosition = Vector3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vTexCoord = pVertices[4].vPosition;

	m_vecVertexPos[5] = pVertices[5].vPosition = Vector3(0.5f, 0.5f, 0.5f);
	pVertices[5].vTexCoord = pVertices[5].vPosition;

	m_vecVertexPos[6] = pVertices[6].vPosition = Vector3(0.5f, -0.5f, 0.5f);
	pVertices[6].vTexCoord = pVertices[6].vPosition;

	m_vecVertexPos[7] = pVertices[7].vPosition = Vector3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vTexCoord = pVertices[7].vPosition;

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

	/* +X */
	m_vecIndex[0] = pIndices[0] = 1;
	m_vecIndex[1] = pIndices[1] = 5;
	m_vecIndex[2] = pIndices[2] = 6;

	m_vecIndex[3] = pIndices[3] = 1;
	m_vecIndex[4] = pIndices[4] = 6;
	m_vecIndex[5] = pIndices[5] = 2;

	/* -X */
	m_vecIndex[6] = pIndices[6] = 4;
	m_vecIndex[7] = pIndices[7] = 0;
	m_vecIndex[8] = pIndices[8] = 3;

	m_vecIndex[9] = pIndices[9] = 4;
	m_vecIndex[10] = pIndices[10] = 3;
	m_vecIndex[11] = pIndices[11] = 7;

	/* +Y */
	m_vecIndex[12] = pIndices[12] = 4;
	m_vecIndex[13] = pIndices[13] = 5;
	m_vecIndex[14] = pIndices[14] = 1;

	m_vecIndex[15] = pIndices[15] = 4;
	m_vecIndex[16] = pIndices[16] = 1;
	m_vecIndex[17] = pIndices[17] = 0;

	/* -Y */
	m_vecIndex[18] = pIndices[18] = 3;
	m_vecIndex[19] = pIndices[19] = 2;
	m_vecIndex[20] = pIndices[20] = 6;

	m_vecIndex[21] = pIndices[21] = 3;
	m_vecIndex[22] = pIndices[22] = 6;
	m_vecIndex[23] = pIndices[23] = 7;

	/* +Z */
	m_vecIndex[24] = pIndices[24] = 5;
	m_vecIndex[25] = pIndices[25] = 4;
	m_vecIndex[26] = pIndices[26] = 7;

	m_vecIndex[27] = pIndices[27] = 5;
	m_vecIndex[28] = pIndices[28] = 7;
	m_vecIndex[29] = pIndices[29] = 6;

	/* -Z */
	m_vecIndex[30] = pIndices[30] = 0;
	m_vecIndex[31] = pIndices[31] = 1;
	m_vecIndex[32] = pIndices[32] = 2;

	m_vecIndex[33] = pIndices[33] = 0;
	m_vecIndex[34] = pIndices[34] = 2;
	m_vecIndex[35] = pIndices[35] = 3;

	m_InitialData.pSysMem = pIndices;

	HRESULT resultIB = __super::Create_Buffer(m_pIB.GetAddressOf());

	Safe_Delete_Array(pIndices);

	if (FAILED(resultIB)) return E_FAIL;
#pragma endregion

	return S_OK;
}

shared_ptr<CCubeProp> CCubeProp::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CCubeProp> pInstance = make_shared<CCubeProp>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCubeProp");
		pInstance.reset();
	}

	return pInstance;
}
