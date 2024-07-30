#pragma once
#include "VIBuffer.h"

namespace Engine
{
    class CAnimModelProp;
    class CBone;

    class CMesh final : public CVIBuffer
    {
    public:
        CMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CMesh() = default;

    public:
        _uint Get_MaterialIndex() const { return m_iMaterialIndex; }
        _uint Get_NumBones() const { return m_iNumBones; }

    public:
        HRESULT Initialize(MeshType eType, const wstring& filePath, Matrix matPivot, _uint& outFilePointer);

    public:
        void SetUp_BoneMatrices(vector<Matrix>& vecBoneMatrices, const vector<shared_ptr<CBone>>& Bones);

    private:
        HRESULT Init_Vertex_Buffer_NonAnim(const wstring& filePath, Matrix matPivot, _uint& outFilePointer);
        HRESULT Init_Vertex_Buffer_Anim(const wstring& filePath, _uint& outFilePointer);

    private:
        _char   m_szName[MAX_PATH] = "";
        _uint   m_iMaterialIndex = 0;

        // 메시별 이용하는 뼈들
        _uint           m_iNumBones = 0;
        vector<_uint>   m_BoneIndices;
        vector<Matrix>  m_OffsetMatrices;

    public:
        static shared_ptr<CMesh> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, MeshType eType, const wstring& filePath, Matrix matPivot, _uint& outFilePointer);
    };
}


