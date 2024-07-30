#pragma once
#include "MovableObject.h"

namespace Engine
{
    class CBone;
}

namespace Client
{
    class CTrailFX;

    class CWeapon : public CGameObject
    {
    public:
        CWeapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CWeapon() = default;

    public:
        virtual HRESULT Initialize(LEVEL eLevel, const wstring& strModelKey, shared_ptr<CBone> socketBone, Vector3 vInitPos, MeshType eMeshType, _bool useTrail);
        virtual void Priority_Tick(_float fTimeDelta) override;
        virtual _int Tick(_float fTimeDelta) override;
        virtual void Late_Tick(_float fTimeDelta) override;
        virtual HRESULT Render() override;
        virtual HRESULT Render_Shadow() override;

    public:
        void Change_Anim(_int AnimIndex);
        void Play_Trail();
        void Stop_Trail();

    protected:
        HRESULT Add_Child();
        HRESULT Add_Component(LEVEL eLevel, const wstring& strModelKey, MeshType eMeshType);
        HRESULT Bind_ShaderResources();
    protected:
        shared_ptr<CSkeletalMeshCom>      m_pAnimModelCom = nullptr;
        shared_ptr<CStaticMeshCom>        m_pNonAnimModelCom = nullptr;
        shared_ptr<CTrailFX>              m_pTrailEffect = nullptr;

    protected:
        MeshType m_eMeshType = MeshType::TYPE_END;

        Vector3 m_vRootMotionVelocity = {};
        _double m_dCurrentAnimFrame = 0.0;
        _bool m_bLastAnimFrameCalled = false;
        _bool m_bUltimateMode = false;

        weak_ptr<CBone> m_SocketBone;
        Matrix m_WorldMatrix = {};
        _bool m_bUseTrail = false;

    public:
        static shared_ptr<CWeapon> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eLevel, const wstring& strModelKey, shared_ptr<CBone> socketBone, MeshType eMeshType, _bool useTrail = false, const Vector3& vInitPos = Vector3(0.f, 0.f, 0.f));
    };
}


