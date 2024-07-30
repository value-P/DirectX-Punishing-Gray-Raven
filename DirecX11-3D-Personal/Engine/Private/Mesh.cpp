#include "Mesh.h"
#include "Bone.h"
#include "AnimModelProp.h"

Engine::CMesh::CMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CVIBuffer(pDevice, pContext)
{
}

HRESULT Engine::CMesh::Initialize(MeshType eType, const wstring& filePath, Matrix matPivot, _uint& outFilePointer)
{
    ifstream istream(filePath, ios::binary);
    if (istream.fail()) return E_FAIL;

    istream.seekg(outFilePointer);
    
    // 이름 받아오기
    _uint iNumString = 0;
    istream.read((_char*)&iNumString, sizeof(_uint));
    _char* pName = new _char[iNumString];
    istream.read(pName, sizeof(_char) * iNumString);
    strcpy_s(m_szName, pName);
    Safe_Delete_Array(pName);

    m_iNumVertexBuffers = 1;
    istream.read((_char*)&m_iNumVertices, sizeof(_uint));
    
    m_iIndexStride = 4;
    m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    istream.read((_char*)&m_iNumIndices, sizeof(_uint));
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    istream.read((_char*)&m_iMaterialIndex, sizeof(_uint));

    outFilePointer = (_uint)istream.tellg();

#pragma region VERTEX_BUFFER
    HRESULT hr = eType == MeshType::NON_ANIM ?
        Init_Vertex_Buffer_NonAnim(filePath, matPivot, outFilePointer) :
        Init_Vertex_Buffer_Anim(filePath, outFilePointer);

    if (FAILED(hr))
    {
        istream.close();
        return E_FAIL;
    }
#pragma endregion

    istream.seekg(outFilePointer);

#pragma region INDEX_BUFFER
    // 인덱스 버퍼를 만든다
    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = 0;

    _uint* pIndices = new _uint[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

    istream.read((_char*)&pIndices[0], sizeof(_uint) * m_iNumIndices);

    m_vecIndex.resize(m_iNumIndices);
    for (_uint i = 0; i < m_iNumIndices; i++)
        m_vecIndex[i] = pIndices[i];

    m_InitialData.pSysMem = pIndices;

    hr = __super::Create_Buffer(&m_pIB);
    Safe_Delete_Array(pIndices);

    if (FAILED(hr))
    {
        istream.close();
        return E_FAIL;
    }

#pragma endregion

    outFilePointer = (_uint)istream.tellg();

    if (eType == MeshType::ANIM)
    {
        istream.seekg(outFilePointer);

        istream.read((_char*)&m_iNumBones, sizeof(_uint));
        m_BoneIndices.resize(m_iNumBones);
        istream.read((_char*)&m_BoneIndices[0], sizeof(_uint) * m_iNumBones);
        m_OffsetMatrices.resize(m_iNumBones);
        istream.read((_char*)&m_OffsetMatrices[0], sizeof(Matrix) * m_iNumBones);

        outFilePointer = (_uint)istream.tellg();
    }

    istream.close();
    return S_OK;
}

void Engine::CMesh::SetUp_BoneMatrices(vector<Matrix>& vecBoneMatrices, const vector<shared_ptr<CBone>>& Bones)
{
    for (_uint i = 0; i < m_iNumBones; i++)
    {
        vecBoneMatrices[i] = m_OffsetMatrices[i] * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix();
    }
}

HRESULT Engine::CMesh::Init_Vertex_Buffer_NonAnim(const wstring& filePath, Matrix matPivot, _uint& outFilePointer)
{
    ifstream istream(filePath, ios::binary);
    if (istream.fail()) return E_FAIL;

    istream.seekg(outFilePointer);

    m_iVertexStride = sizeof(VTXMESH);
    // 정점 버퍼를 만든다

    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = m_iVertexStride;

    VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

    istream.read((_char*)pVertices, sizeof(VTXMESH) * m_iNumVertices);

    m_vecVertexPos.resize(m_iNumVertices);
    for (size_t i = 0; i < m_iNumVertices; i++)
        m_vecVertexPos[i] = pVertices[i].vPosition;

    m_InitialData.pSysMem = pVertices;

    HRESULT hr = __super::Create_Buffer(&m_pVB);
    Safe_Delete_Array(pVertices);
    
    if (FAILED(hr))
    {
        istream.close();
        return E_FAIL;
    }
    
    outFilePointer = (_uint)istream.tellg();
    istream.close();

    return S_OK;
}

HRESULT Engine::CMesh::Init_Vertex_Buffer_Anim(const wstring& filePath, _uint& outFilePointer)
{
    ifstream istream(filePath, ios::binary);
    if (istream.fail()) return E_FAIL;

    istream.seekg(outFilePointer);

    m_iVertexStride = sizeof(VTXANIMMESH);
    // 정점 버퍼를 만든다
    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = m_iVertexStride;

    VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

    istream.read((_char*)pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

    m_vecVertexPos.resize(m_iNumVertices);
    for (size_t i = 0; i < m_iNumVertices; i++)
        m_vecVertexPos[i] = pVertices[i].vPosition;

    m_InitialData.pSysMem = pVertices;

    HRESULT hr = __super::Create_Buffer(&m_pVB);
    Safe_Delete_Array(pVertices);

    if (FAILED(hr))
    {
        istream.close();
        return E_FAIL;
    }

    outFilePointer = (_uint)istream.tellg();
    istream.close();

    return S_OK;
}

shared_ptr<CMesh> Engine::CMesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, MeshType eType, const wstring& filePath, Matrix matPivot, _uint& outFilePointer)
{
    shared_ptr<CMesh> pInstance = make_shared<CMesh>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(eType, filePath, matPivot, outFilePointer)))
    {
        MSG_BOX("Failed to Created : CMesh");
        pInstance.reset();
    }

    return pInstance;
}
