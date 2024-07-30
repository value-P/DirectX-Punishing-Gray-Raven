#pragma once
#include "PrimitiveComponent.h"

namespace Engine
{
    class CAnimModelProp;
    class CBone;

    class ENGINE_DLL CSkeletalMeshCom :
        public CPrimitiveComponent
    {
    public:
        CSkeletalMeshCom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CSkeletalMeshCom() = default;

    public:
        _int Get_MeshCount();
        _int Get_AnimCount();
        _int Get_CurrentAnimIndex();
        const _char* Get_AnimName();
        const _char* Get_AnimName(_int iAnimIndex);
        _int Get_MaxKeyFrame();
        _int Get_CurrentKeyFrame();
        shared_ptr<CBone> Get_Bone(const _char* pBoneName);

        void Set_KeyFrame(_int iKeyFrame);
        void Set_Animation(_uint iAnimIndex);
        void Set_AnimationNext();
        void Set_AnimationPrev();

    public:
        HRESULT Initialize(shared_ptr<CAnimModelProp> pModelProp);
        HRESULT Render(_uint iMeshIndex, _uint iPassIndex, _uint iTechniqueIndex = 0);

    public:
        HRESULT Bind_SRV(const _char* pConstantName, _uint iMeshIndex, MaterialTexType eMatrialType);
        HRESULT Bind_BoneMatrices(const _char* pConstantName, _uint iMeshIndex);

        void Play_Animation(_float fTimeDelta, Vector3& outMoveVelocity, _bool& outLastFrameCalled, _double& outFrameRate, _bool useLinearChange, _bool enableRootMotion = true, _bool isLoop = false);

        virtual _bool Check_RayPicked(const Ray& ray, _float& fOutDist) override;

    private:
        shared_ptr<CAnimModelProp> m_pModelProp;

    public:
        static shared_ptr<CSkeletalMeshCom> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<CAnimModelProp> pAnimModelProp);

    };
}