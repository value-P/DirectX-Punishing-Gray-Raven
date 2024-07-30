#include "pch.h"
#include "LaserObject.h"
#include "MovableObject.h"
#include "Bone.h"
#include "MeshEffect.h"

Client::CLaserObject::CLaserObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CEffectObject(pDevice, pContext)
{
}

HRESULT Client::CLaserObject::Initialize(const INIT_DESC& initDesc)
{
    if (FAILED(__super::Initialize(initDesc)))
        return E_FAIL;

    if (FAILED(Add_Component(initDesc)))
        return E_FAIL;

    if (initDesc.pSocketBone != nullptr)
        m_pSocketBone = initDesc.pSocketBone;

    if (initDesc.strSkillPrefabPath != L"")
    {
        CMeshEffect::INIT_DESC desc;
        desc.eLevel = initDesc.eCreateLevel;
        desc.FXPrefabPath = initDesc.strSkillPrefabPath;
        desc.vInitPos = Vector3(0.f, 0.f, 0.f);

        m_pMeshEffect = CMeshEffect::Create(m_pDevice, m_pContext, desc);
        m_pGameInstance.lock()->Add_Object(initDesc.eCreateLevel, L"Layer_MonsterSkill", m_pMeshEffect, &g_TimeScale_Monster);

    }

    return S_OK;
}

void Client::CLaserObject::Priority_Tick(_float fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);

    if (nullptr != m_pMeshEffect)
        m_pMeshEffect->Priority_Tick(fTimeDelta);

}

_int Client::CLaserObject::Tick(_float fTimeDelta)
{
    if (m_pCollider->IsActivate() == false) 
        return OBJ_ALIVE;

    __super::Tick(fTimeDelta);

    if (m_pCollider->IsActivate() && m_bUseRangeRotation)
    {
        _float timeRate = (m_fDuration - m_fDurationCheck) / m_fDuration;
        _float currentRad = m_fStartLook + (m_fDestLook - m_fStartLook) * timeRate;
        _float degree = XMConvertToDegrees(currentRad);
        m_pTransformCom->Rotation(XMVectorSetW(Vector4(1.f,0.f,0.f,0.f), 0.f), currentRad);
    }

    if (m_pCollider->IsActivate())
    {
        m_fDurationCheck -= fTimeDelta;
        if (m_fDurationCheck <= 0.f)
            InActivate();

        if (nullptr != m_pParentTransform.lock())
        {
            if (m_pSocketBone.lock() != nullptr)
            {
                _float y = m_pSocketBone.lock()->Get_CombinedTransformationMatrix().Translation().y;
                m_pTransformCom->Set_Y(y);
                m_matWorld = m_pTransformCom->Get_WorldMatrix() * m_pParentTransform.lock()->Get_WorldMatrix();
            }
            else
                m_matWorld = m_pTransformCom->Get_WorldMatrix() * m_pParentTransform.lock()->Get_WorldMatrix();
        }
        else
            m_matWorld = m_pTransformCom->Get_WorldMatrix();

        m_pCollider->Tick(m_matWorld);
    }

    if (nullptr != m_pMeshEffect)
        m_pMeshEffect->Tick(fTimeDelta, m_matWorld);

    return OBJ_ALIVE;
}

void Client::CLaserObject::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    if (nullptr != m_pMeshEffect)
        m_pMeshEffect->Late_Tick(fTimeDelta);

    m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_BLEND, shared_from_this());
}

HRESULT Client::CLaserObject::Render()
{
    __super::Render();

    return S_OK;
}

void Client::CLaserObject::LookAt(const Vector3& vTargetPos)
{
    m_pTransformCom->LookAt(XMVectorSetW(vTargetPos,1.f));
}

void Client::CLaserObject::ActiveRotationRage(_float fStartRad, _float fDestRad)
{
    m_bUseRangeRotation = true;
    m_fStartLook = fStartRad;
    m_fDestLook = fDestRad;
    Activate();

    if (m_pMeshEffect != nullptr)
        m_pMeshEffect->PlayOnce();
}

HRESULT Client::CLaserObject::Add_Component(const INIT_DESC& initDesc)
{
    CBounding_OBB::INIT_DESC obbDesc;
    obbDesc.vCenter = initDesc.vColOffset;
    obbDesc.vExtents = initDesc.vColExtents;

    CCollider::INIT_DESC colliderDesc;
    colliderDesc.eType = CCollider::TYPE_OBB;
    colliderDesc.iLayerType = Col_MonsterAttack;
    colliderDesc.pInitDesc = make_shared<CBounding_OBB::INIT_DESC>(obbDesc);
    colliderDesc.pOwner = shared_from_this();
    colliderDesc.iCreateLevelIndex = initDesc.eCreateLevel;
    m_pCollider = CCollider::Create(m_pDevice, m_pContext, colliderDesc);
    m_pCollider->Set_Activate(false);

    return S_OK;
}

HRESULT Client::CLaserObject::Bind_ShaderResources()
{
    return S_OK;
}

void Client::CLaserObject::InActivate()
{
    __super::InActivate();
    m_bUseRangeRotation = false;
}

void Client::CLaserObject::OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
{
    if (pCollider->Get_ColliderLayer() == Col_Player)
    {
        static_pointer_cast<CMovableObject>(pCollider->Get_Owner().lock())->Hit(m_iDamage);
    }
}

shared_ptr<CLaserObject> Client::CLaserObject::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc)
{
    shared_ptr<CLaserObject> pInstance = make_shared<CLaserObject>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(initDesc)))
    {
        MSG_BOX("Failed to Created : CLaserObject");
        pInstance.reset();
    }

    return pInstance;
}
