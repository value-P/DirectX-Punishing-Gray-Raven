#pragma once
#include "EffectProp.h"

namespace Engine
{
    class CAnimModelProp;

    class ENGINE_DLL CAnimEffectProp final : public CEffectProp
    {
    public:
        CAnimEffectProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        CAnimEffectProp(const CAnimEffectProp& rhs);
        virtual ~CAnimEffectProp() = default;

    public:
        _bool isAnimMesh();
        _int Get_MeshCount();
        ComPtr<ID3D11ShaderResourceView> Get_MaterialSRV(_uint iMeshIndex, MaterialTexType eMaterialType) const;

        void SetKeyFrame(_int iKeyFrame);

        virtual HRESULT Add_Buffer(const wstring& strBufferPath, const Matrix& pivotMatrix) override;
        vector<Matrix> Get_BoneMatrices(_uint iMeshIndex);

        shared_ptr<CAnimModelProp> Get_Buffer() { return m_pModelProp; }

    public:
        virtual HRESULT Initialize() override;
        void Play_Animation(_float fTimeDelta, _bool& lastFrameCalled, _double& fFrameRate, _bool isLoop);

        virtual HRESULT Render(_uint iMeshIndex);

    private:
        shared_ptr<CAnimModelProp> m_pModelProp = nullptr;

    public:
        static shared_ptr<CAnimEffectProp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        shared_ptr<CAnimEffectProp> Clone();
    };
}


