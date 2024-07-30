#include "TrailBufferPop.h"

Engine::CTrailBufferPop::CTrailBufferPop(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CVIBuffer(pDevice, pContext)
{
}

HRESULT Engine::CTrailBufferPop::Initialize(_uint iNumTrailRect)
{
	m_iNumTrailRect = iNumTrailRect;

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumVertices = (m_iNumTrailRect + 1) * 2; // 사각형 2개 = 정점6개
	m_vecVertexPos.resize(m_iNumVertices);
	
	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = m_iNumTrailRect * 6;
	m_vecIndex.resize(m_iNumIndices);
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	HRESULT hr;

#pragma region VERTEX_BUFFER
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	m_InitialData.pSysMem = pVertices;

	hr = __super::Create_Buffer(m_pVB.GetAddressOf());
	Safe_Delete_Array(pVertices);

	if (FAILED(hr))
		return E_FAIL;
#pragma endregion
	
	// 정점버퍼 순서
	// 칼Up		... 5 3 1
	// 칼Down	... 4 2 0

#pragma region INDEX_BUFFER
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	_int iRectCnt = 0;
	for (_ushort iIndexCnt = 0; iIndexCnt < m_iNumIndices;)
	{
		_ushort firstVertexIndex = iRectCnt * 2;

		m_vecIndex[iIndexCnt] = firstVertexIndex + 3;
		pIndices[iIndexCnt++] = firstVertexIndex + 3;

		m_vecIndex[iIndexCnt] = firstVertexIndex + 1;
		pIndices[iIndexCnt++] = firstVertexIndex + 1;

		m_vecIndex[iIndexCnt] = firstVertexIndex;
		pIndices[iIndexCnt++] = firstVertexIndex;

		// 아래삼각형

		m_vecIndex[iIndexCnt] = firstVertexIndex + 3;
		pIndices[iIndexCnt++] = firstVertexIndex + 3;

		m_vecIndex[iIndexCnt] = firstVertexIndex;
		pIndices[iIndexCnt++] = firstVertexIndex;

		m_vecIndex[iIndexCnt] = firstVertexIndex + 2;
		pIndices[iIndexCnt++] = firstVertexIndex + 2;

		iRectCnt++;
	}

	m_InitialData.pSysMem = pIndices;

	hr = __super::Create_Buffer(m_pIB.GetAddressOf());
	Safe_Delete_Array(pIndices);

	if (FAILED(hr))
		return E_FAIL;
#pragma endregion

    return S_OK;
}

// 월드의 두 정점을 받아서 트레일의 정점포지션 변경
void Engine::CTrailBufferPop::Tick(const Vector3& vPosUpperWorld, const Vector3& vPosDownWorld)
{
	TrailBase base;
	base.vUppderPos = vPosUpperWorld;
	base.vDownPos = vPosDownWorld;

	m_TrailVertexPositions.push_back(base);

	if (m_TrailVertexPositions.size() > m_iNumTrailRect + 1)
		m_TrailVertexPositions.pop_front();

	// 정점버퍼 순서
	// 칼Up		... 5 3 1
	// 칼Down	... 4 2 0

	// 정점 4개이상일때부터 데이터가 들어가도록 : TrailPosition은 가장 앞에 있는것이 가장 칼에서 먼 면
	if (m_TrailVertexPositions.size() > 2)
	{
		D3D11_MAPPED_SUBRESOURCE subResource;

		m_pContext->Map(m_pVB.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);
		VTXPOSTEX* pVertices = reinterpret_cast<VTXPOSTEX*>(subResource.pData);

		// 들어온 포지션만큼 정점에 적용
		_ushort iVertexCnt = 0;
		for (auto& TrailVertexPosition : m_TrailVertexPositions)
		{
			// 0
			_float URate = static_cast<_float>(iVertexCnt) / (m_iNumVertices - 2);
			m_vecVertexPos[iVertexCnt] = pVertices[iVertexCnt].vPosition = TrailVertexPosition.vDownPos;
			pVertices[iVertexCnt].vTexCoord = Vector2(1.f - URate, 1.f);

			++iVertexCnt;
			// 1
			m_vecVertexPos[iVertexCnt] = pVertices[iVertexCnt].vPosition = TrailVertexPosition.vUppderPos;
			pVertices[iVertexCnt].vTexCoord = Vector2(1.f - URate, 0.f);

			++iVertexCnt;
		}

		//// 트레일이 최소 10개 이상이며 지정한 숫자만큼 꽉 차있다면 보간
		//if (m_TrailVertexPositions.size() > 5 &&
		//	m_TrailVertexPositions.size() >= m_iNumTrailRect + 1)
		//{
		//	_int iCatMullRomCnt = (m_iNumTrailRect + 1) - 4;
		//
		//	// 아래곡면 보간
		//	Vector3 V2 = m_vecVertexPos[2];
		//	Vector3 V1 = m_vecVertexPos[0];
		//	Vector3 V3 = m_vecVertexPos[m_iNumVertices - 4];
		//	Vector3 V4 = m_vecVertexPos[m_iNumVertices - 2];
		//	
		//	for (_int iIndex = 0; iIndex < iCatMullRomCnt; ++iIndex)
		//	{
		//		_float t = (_float)(iIndex) / (_float)(iCatMullRomCnt);
		//		_int vertexIdx = (iIndex + 2) * 2;
		//		pVertices[vertexIdx].vPosition = Vector3::CatmullRom(V1, V2, V3, V4, t);
		//		int abc = 12;
		//	}
		//
		//	// 윗곡면 보간
		//	V1 = m_vecVertexPos[1];
		//	V2 = m_vecVertexPos[3];
		//	V3 = m_vecVertexPos[m_iNumVertices - 3];
		//	V4 = m_vecVertexPos[m_iNumVertices - 1];
		//
		//	for (_int iIndex = 0; iIndex < iCatMullRomCnt; ++iIndex)
		//	{
		//		_float t = (_float)(iIndex) / (_float)(iCatMullRomCnt);
		//		_int vertexIdx = (iIndex + 2) * 2 + 1;
		//		pVertices[vertexIdx].vPosition = Vector3::CatmullRom(V1, V2, V3, V4, t);
		//	}
		//}

		m_pContext->Unmap(m_pVB.Get(), 0);
	}
}

shared_ptr<CTrailBufferPop> Engine::CTrailBufferPop::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumTrailRect)
{
	shared_ptr<CTrailBufferPop> pInstance = make_shared<CTrailBufferPop>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iNumTrailRect)))
	{
		MSG_BOX("Failed to Created : CTrailBufferPop");
		pInstance.reset();
	}

	return pInstance;
}
