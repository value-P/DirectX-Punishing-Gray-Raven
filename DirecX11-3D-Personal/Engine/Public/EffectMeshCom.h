#pragma once
#include "PrimitiveComponent.h"

namespace Engine
{
    class CAnimEffectProp;
    class CBone;
    class CTexture;

    class ENGINE_DLL CEffectMeshCom : public CPrimitiveComponent
    {
    public:
        CEffectMeshCom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CEffectMeshCom() = default;

    public:
        _bool isAnimEffect();
        _int Get_MeshCount();
        Vector2 Get_UVMove() { return m_vUVMove; }
        void Set_UVMoveSpeed(Vector2 vUVSpeed) { m_vUVSpeed = vUVSpeed; }
        void Set_KeyFrame(_int iKeyFrame);

        void Activate() {}

    public:
        HRESULT Initialize(shared_ptr<CAnimEffectProp> pEffectProp);
        HRESULT Render(_uint iMeshIndex, _uint iPassIndex, _uint iTechniqueIndex = 0);

    public:
        HRESULT Bind_SRV(const _char* pConstantName, _uint iMeshIndex, MaterialTexType eMatrialType);
        HRESULT Bind_SRV(const _char* pConstantName, shared_ptr<CTexture> pTex);
        HRESULT Bind_BoneMatrices(const _char* pConstantName, _uint iMeshIndex);

        void Play_Animation(_float fTimeDelta, _bool& lastFrameCalled, _double& fFrameRate, _bool isLoop = true);
        void UV_Move(_float fTimeDelta);

    private:
        shared_ptr<CAnimEffectProp> m_pEffectProp = nullptr;

        _bool m_bActivate = false;

        Vector2 m_vUVMove = {};
        Vector2 m_vUVSpeed = {};

    public:
        static shared_ptr<CEffectMeshCom> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<CAnimEffectProp> pEffectProp);
    };
}