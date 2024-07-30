#include "AnimEffectProp.h"
#include "AnimModelProp.h"

Engine::CAnimEffectProp::CAnimEffectProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CEffectProp(pDevice, pContext)
{
}

Engine::CAnimEffectProp::CAnimEffectProp(const CAnimEffectProp& rhs)
    :CEffectProp(rhs), m_pModelProp(rhs.m_pModelProp)
{
}

_bool Engine::CAnimEffectProp::isAnimMesh()
{
    return m_pModelProp->Get_AnimCount() > 0; 
}

_int Engine::CAnimEffectProp::Get_MeshCount()
{
    return m_pModelProp->Get_NumMeshes();
}

ComPtr<ID3D11ShaderResourceView> Engine::CAnimEffectProp::Get_MaterialSRV(_uint iMeshIndex, MaterialTexType eMaterialType) const
{
    return m_pModelProp->Get_MaterialSRV(iMeshIndex, eMaterialType);
}

void Engine::CAnimEffectProp::SetKeyFrame(_int iKeyFrame)
{
    m_pModelProp->Set_KeyFrame(iKeyFrame);
}

vector<Matrix> Engine::CAnimEffectProp::Get_BoneMatrices(_uint iMeshIndex)
{
    return m_pModelProp->Get_BoneMatrices(iMeshIndex);
}

HRESULT Engine::CAnimEffectProp::Add_Buffer(const wstring& strBufferPath, const Matrix& pivotMatrix)
{
    if (m_pModelProp != nullptr)
        return E_FAIL;

    shared_ptr<CAnimModelProp> pModelProp = CAnimModelProp::Create(m_pDevice, m_pContext, strBufferPath, pivotMatrix);
    if (nullptr == pModelProp)
        return E_FAIL;

    m_pModelProp = pModelProp;

    return S_OK;
}

HRESULT Engine::CAnimEffectProp::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    return S_OK;
}

void Engine::CAnimEffectProp::Play_Animation(_float fTimeDelta, _bool& lastFrameCalled, _double& fFrameRate, _bool isLoop)
{
    Vector3 moveVelocity;
    m_pModelProp->Play_Animation(fTimeDelta, isLoop, moveVelocity, lastFrameCalled, fFrameRate, false, false);
}

HRESULT Engine::CAnimEffectProp::Render(_uint iMeshIndex)
{
    return m_pModelProp->Render(iMeshIndex);
}

shared_ptr<CAnimEffectProp> Engine::CAnimEffectProp::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CAnimEffectProp> pInstance = make_shared<CAnimEffectProp>(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CAnimEffectProp");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CAnimEffectProp> Engine::CAnimEffectProp::Clone()
{
    return make_shared<CAnimEffectProp>(*this);
}
