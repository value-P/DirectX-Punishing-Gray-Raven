#include "SkeletalMeshCom.h"
#include "AnimModelProp.h"
#include "Shader.h"
#include "Bone.h"

Engine::CSkeletalMeshCom::CSkeletalMeshCom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CPrimitiveComponent(pDevice, pContext)
{
}

_int Engine::CSkeletalMeshCom::Get_MeshCount()
{
    return m_pModelProp->Get_NumMeshes();
}

_int Engine::CSkeletalMeshCom::Get_AnimCount()
{
    return m_pModelProp->Get_AnimCount();
}

_int Engine::CSkeletalMeshCom::Get_CurrentAnimIndex()
{
    return m_pModelProp->Get_CurrentAnimIndex();
}

const _char* Engine::CSkeletalMeshCom::Get_AnimName()
{
    return m_pModelProp->Get_AnimName(Get_CurrentAnimIndex());
}

const _char* Engine::CSkeletalMeshCom::Get_AnimName(_int iAnimIndex)
{
    return m_pModelProp->Get_AnimName(iAnimIndex);
}

_int Engine::CSkeletalMeshCom::Get_MaxKeyFrame()
{
    return m_pModelProp->Get_MaxKeyFrame(Get_CurrentAnimIndex());
}

_int Engine::CSkeletalMeshCom::Get_CurrentKeyFrame()
{
    return m_pModelProp->Get_CurrentKeyFrame(Get_CurrentAnimIndex());
}

shared_ptr<CBone> Engine::CSkeletalMeshCom::Get_Bone(const _char* pBoneName)
{
    return m_pModelProp->Get_Bone(pBoneName);
}

void Engine::CSkeletalMeshCom::Set_KeyFrame(_int iKeyFrame)
{
    m_pModelProp->Set_KeyFrame(iKeyFrame);
}

void Engine::CSkeletalMeshCom::Set_Animation(_uint iAnimIndex)
{
    m_pModelProp->Set_Animataion(iAnimIndex);
}

void Engine::CSkeletalMeshCom::Set_AnimationNext()
{
    m_pModelProp->Set_AnimationNext();
}

void Engine::CSkeletalMeshCom::Set_AnimationPrev()
{
    m_pModelProp->Set_AnimationPrev();
}

HRESULT Engine::CSkeletalMeshCom::Initialize(shared_ptr<CAnimModelProp> pModelProp)
{
    if (pModelProp == nullptr)
        return E_FAIL;

    m_pModelProp = pModelProp->Clone();

    return S_OK;
}

HRESULT Engine::CSkeletalMeshCom::Render(_uint iMeshIndex, _uint iPassIndex, _uint iTechniqueIndex)
{
    if (iMeshIndex >= m_pModelProp->Get_NumMeshes())
        return E_FAIL;

    if (FAILED(m_pShader->Begin(iPassIndex, iTechniqueIndex)))
        return E_FAIL;

    m_pModelProp->Render(iMeshIndex);

    return S_OK;
}

HRESULT Engine::CSkeletalMeshCom::Bind_SRV(const _char* pConstantName, _uint iMeshIndex, MaterialTexType eMatrialType)
{
    if (nullptr == m_pShader || nullptr == m_pModelProp) return E_FAIL;

    ComPtr<ID3D11ShaderResourceView> pSRV = m_pModelProp->Get_MaterialSRV(iMeshIndex, eMatrialType);

    if (nullptr == pSRV) return S_OK;

    return m_pShader->Bind_SRV(pConstantName, pSRV);
}

HRESULT Engine::CSkeletalMeshCom::Bind_BoneMatrices(const _char* pConstantName, _uint iMeshIndex)
{
    return m_pShader->Bind_Matrices(pConstantName, m_pModelProp->Get_BoneMatrices(iMeshIndex).data(), 512);
}

void Engine::CSkeletalMeshCom::Play_Animation(_float fTimeDelta, Vector3& outMoveVelocity, _bool& outLastFrameCalled, _double& outFrameRate, _bool useLinearChange, _bool enableRootMotion, _bool isLoop)
{
    m_pModelProp->Play_Animation(fTimeDelta, isLoop, outMoveVelocity, outLastFrameCalled, outFrameRate, enableRootMotion, useLinearChange);
}

_bool Engine::CSkeletalMeshCom::Check_RayPicked(const Ray& ray, _float& fOutDist)
{
    return m_pModelProp->Check_RayPicked(ray, fOutDist);
}

shared_ptr<CSkeletalMeshCom> Engine::CSkeletalMeshCom::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<CAnimModelProp> pAnimModelProp)
{
    shared_ptr<CSkeletalMeshCom> pInstance = make_shared<CSkeletalMeshCom>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pAnimModelProp)))
    {
        MSG_BOX("Failed to Created : CSkeletalMeshCom");
        pInstance.reset();
    }

    return pInstance;
}
