#include "TerrainProp.h"

Engine::CTerrainProp::CTerrainProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CVIBuffer(pDevice, pContext)
{
}

HRESULT Engine::CTerrainProp::Initialize(const wstring& strHeightMapPath)
{
	ifstream fin;
	fin.open(strHeightMapPath, ios::binary);
	if (!fin.is_open()) return E_FAIL;

	BITMAPFILEHEADER bf;
	fin.read((char*)&bf, sizeof(bf));

	BITMAPINFOHEADER bi;
	fin.read((char*)&bi, sizeof(bi));

	_uint* pPixels = new _uint[bi.biWidth * bi.biHeight];
	fin.read((char*)pPixels, sizeof(_uint) * bi.biWidth * bi.biHeight);
	fin.close();

	m_iNumVerticesX = bi.biWidth;
	m_iNumVerticesZ = bi.biHeight;

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_vecVertexPos.resize(m_iNumVertices);

	m_iIndexStride = 4;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_vecIndex.resize(m_iNumIndices);
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (_uint z = 0; z < m_iNumVerticesZ; ++z)
	{
		for (_uint x = 0; x < m_iNumVerticesX; ++x)
		{
			_uint iIndex = z * m_iNumVerticesX + x;

			m_vecVertexPos[iIndex] = pVertices[iIndex].vPosition = { (_float)x, (pPixels[iIndex] & 0x000000ff) * 0.1f, (_float)z };
			pVertices[iIndex].vNormal = { 0.f, 0.f, 0.f };
			pVertices[iIndex].vTexCoord = { (_float)x / (m_iNumVerticesX - 1), (_float)z / (m_iNumVerticesZ - 1) };
		}
	}

	Safe_Delete_Array(pPixels);

#pragma endregion

#pragma region INDEX_BUFFER

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint iIndexCnt = 0;
	for (_uint z = 0; z < m_iNumVerticesZ - 1; ++z)
	{
		for (_uint x = 0; x < m_iNumVerticesX - 1; ++x)
		{
			_uint iIndex = z * m_iNumVerticesX + x;

			_uint iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			Vector3 vSourDir, vDestDir, vNormal;

			m_vecIndex[iIndexCnt] = pIndices[iIndexCnt] = iIndices[0];
			iIndexCnt++;
			m_vecIndex[iIndexCnt] = pIndices[iIndexCnt] = iIndices[1];
			iIndexCnt++;
			m_vecIndex[iIndexCnt] = pIndices[iIndexCnt] = iIndices[2];
			iIndexCnt++;

			vSourDir = pVertices[iIndices[1]].vPosition - pVertices[iIndices[0]].vPosition;
			vDestDir = pVertices[iIndices[2]].vPosition - pVertices[iIndices[1]].vPosition;
			vNormal = vSourDir.Cross(vDestDir);
			vNormal.Normalize();
			
			pVertices[iIndices[0]].vNormal += vNormal;
			pVertices[iIndices[0]].vNormal.Normalize();
			pVertices[iIndices[1]].vNormal += vNormal;
			pVertices[iIndices[1]].vNormal.Normalize();
			pVertices[iIndices[2]].vNormal += vNormal;
			pVertices[iIndices[2]].vNormal.Normalize();

			m_vecIndex[iIndexCnt] = pIndices[iIndexCnt] = iIndices[0];
			iIndexCnt++;
			m_vecIndex[iIndexCnt] = pIndices[iIndexCnt] = iIndices[2];
			iIndexCnt++;
			m_vecIndex[iIndexCnt] = pIndices[iIndexCnt] = iIndices[3];
			iIndexCnt++;

			vSourDir = pVertices[iIndices[2]].vPosition - pVertices[iIndices[0]].vPosition;
			vDestDir = pVertices[iIndices[3]].vPosition - pVertices[iIndices[2]].vPosition;
			vNormal = vSourDir.Cross(vDestDir);
			vNormal.Normalize();

			pVertices[iIndices[0]].vNormal += vNormal;
			pVertices[iIndices[0]].vNormal.Normalize();
			pVertices[iIndices[2]].vNormal += vNormal;
			pVertices[iIndices[2]].vNormal.Normalize();
			pVertices[iIndices[3]].vNormal += vNormal;
			pVertices[iIndices[3]].vNormal.Normalize();
		}
	}

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_InitialData.pSysMem = pVertices;

	HRESULT resultVB = __super::Create_Buffer(m_pVB.GetAddressOf());

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	m_InitialData.pSysMem = pIndices;

	HRESULT resultIB = __super::Create_Buffer(m_pIB.GetAddressOf());

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	if (FAILED(resultIB) || FAILED(resultVB)) return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT Engine::CTerrainProp::Initialize(_int iSizeX, _int iSizeZ)
{
	m_iNumVerticesX = iSizeX;
	m_iNumVerticesZ = iSizeZ;

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_vecVertexPos.resize(m_iNumVertices);

	m_iIndexStride = 4;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_vecIndex.resize(m_iNumIndices);

#pragma region VERTEX_BUFFER
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (_uint z = 0; z < m_iNumVerticesZ; ++z)
	{
		for (_uint x = 0; x < m_iNumVerticesX; ++x)
		{
			_uint iIndex = z * m_iNumVerticesX + x;

			m_vecVertexPos[iIndex] = pVertices[iIndex].vPosition = { (_float)x, 0.f, (_float)z };
			pVertices[iIndex].vNormal = { 0.f,0.f,0.f };
			pVertices[iIndex].vTexCoord = { (_float)x / (m_iNumVerticesX - 1), (_float)z / (m_iNumVerticesZ - 1) };
		}
	}

#pragma endregion

#pragma region INDEX_BUFFER
	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint iIndexCnt = 0;

	for (_uint z = 0; z < m_iNumVerticesZ - 1; ++z)
	{
		for (_uint x = 0; x < m_iNumVerticesX - 1; ++x)
		{
			_uint index = z * m_iNumVerticesX + x;

			_uint Indices[4] = {
				index + m_iNumVerticesX,
				index + m_iNumVerticesX + 1,
				index + 1,
				index
			};

			Vector3 vSourDir, vDestDir, vNormal;

			m_vecIndex[iIndexCnt] = pIndices[iIndexCnt] = Indices[0];
			iIndexCnt++;
			m_vecIndex[iIndexCnt] = pIndices[iIndexCnt] = Indices[1];
			iIndexCnt++;
			m_vecIndex[iIndexCnt] = pIndices[iIndexCnt] = Indices[2];
			iIndexCnt++;

			vSourDir = pVertices[Indices[1]].vPosition - pVertices[Indices[0]].vPosition;
			vDestDir = pVertices[Indices[2]].vPosition - pVertices[Indices[1]].vPosition;
			vNormal = vSourDir.Cross(vDestDir);
			vNormal.Normalize();

			pVertices[Indices[0]].vNormal += vNormal;
			pVertices[Indices[0]].vNormal.Normalize();
			pVertices[Indices[1]].vNormal += vNormal;
			pVertices[Indices[1]].vNormal.Normalize();
			pVertices[Indices[2]].vNormal += vNormal;
			pVertices[Indices[2]].vNormal.Normalize();

			m_vecIndex[iIndexCnt] = pIndices[iIndexCnt] = Indices[0];
			iIndexCnt++;
			m_vecIndex[iIndexCnt] = pIndices[iIndexCnt] = Indices[2];
			iIndexCnt++;
			m_vecIndex[iIndexCnt] = pIndices[iIndexCnt] = Indices[3];
			iIndexCnt++;

			vSourDir = pVertices[Indices[2]].vPosition - pVertices[Indices[0]].vPosition;
			vDestDir = pVertices[Indices[3]].vPosition - pVertices[Indices[2]].vPosition;
			vNormal = vSourDir.Cross(vDestDir);
			vNormal.Normalize();

			pVertices[Indices[0]].vNormal += vNormal;
			pVertices[Indices[0]].vNormal.Normalize();
			pVertices[Indices[2]].vNormal += vNormal;
			pVertices[Indices[2]].vNormal.Normalize();
			pVertices[Indices[3]].vNormal += vNormal;
			pVertices[Indices[3]].vNormal.Normalize();

		}
	}
#pragma endregion

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_InitialData.pSysMem = pVertices;

	HRESULT resultVB = __super::Create_Buffer(m_pVB.GetAddressOf());

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	m_InitialData.pSysMem = pIndices;

	HRESULT resultIB = __super::Create_Buffer(m_pIB.GetAddressOf());

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	if (FAILED(resultIB) || FAILED(resultVB)) return E_FAIL;

	return S_OK;
}

shared_ptr<CTerrainProp> Engine::CTerrainProp::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strHeightMapPath)
{
	shared_ptr<CTerrainProp> pInstance = make_shared<CTerrainProp>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strHeightMapPath)))
	{
		MSG_BOX("Failed to Created : CTerrainProp");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CTerrainProp> Engine::CTerrainProp::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _int iSizeX, _int iSizeZ)
{
	shared_ptr<CTerrainProp> pInstance = make_shared<CTerrainProp>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iSizeX, iSizeZ)))
	{
		MSG_BOX("Failed to Created : CTerrainProp");
		pInstance.reset();
	}

	return pInstance;
}
