#include "pch.h"
#include "Weapon.h"
#include "Bone.h"
#include "TrailFX.h"

Client::CWeapon::CWeapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice, pContext)
{
}

HRESULT Client::CWeapon::Initialize(LEVEL eLevel, const wstring& strModelKey, shared_ptr<CBone> socketBone, Vector3 vInitPos, MeshType eMeshType, _bool useTrail)
{
    m_bUseTrail = useTrail;

    if (FAILED(__super::Initialize(vInitPos)))
        return E_FAIL;

    if (FAILED(Add_Component(eLevel, strModelKey, eMeshType))) return E_FAIL;

    if (m_bUseTrail)
    {
        if (FAILED(Add_Child()))
            return E_FAIL;
    }
    
    m_SocketBone = socketBone;

    return S_OK;
}

void Client::CWeapon::Priority_Tick(_float fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);
}

_int Client::CWeapon::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (m_eMeshType == MeshType::ANIM)
        m_pAnimModelCom->Play_Animation(fTimeDelta, m_vRootMotionVelocity, m_bLastAnimFrameCalled, m_dCurrentAnimFrame, false, false, true);

    return OBJ_ALIVE;
}

void Client::CWeapon::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    m_WorldMatrix = m_pTransformCom->Get_WorldMatrix() * m_SocketBone.lock()->Get_CombinedTransformationMatrix() * m_pParentTransform.lock()->Get_WorldMatrix();
       
    if(m_bUseTrail)
        m_pTrailEffect->Tick(m_WorldMatrix);
}

HRESULT Client::CWeapon::Render()
{
    __super::Render();

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iMeshCount = m_eMeshType == MeshType::ANIM ? m_pAnimModelCom->Get_MeshCount() : m_pNonAnimModelCom->Get_MeshCount();

    for (_uint i = 0; i < iMeshCount; ++i)
    {
        if (m_eMeshType == MeshType::ANIM)
        {
            // 텍스처 바인딩
            if (FAILED(m_pAnimModelCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
                return E_FAIL;

            if (FAILED(m_pAnimModelCom->Bind_BoneMatrices("g_BoneMatrices", i)))
                return E_FAIL;

            m_pAnimModelCom->Render(i, 0);
        }
        else
        {
            // 텍스처 바인딩
            if (FAILED(m_pNonAnimModelCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
                return E_FAIL;

            m_pNonAnimModelCom->Render(i, 0);
        }
    }

    return S_OK;
}

HRESULT Client::CWeapon::Render_Shadow()
{
    Matrix ViewMatrix = *(m_pGameInstance.lock()->Get_LightViewMatrix(0));

    Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

    if (FAILED(m_pNonAnimModelCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pNonAnimModelCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pNonAnimModelCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    _uint iMeshCount = m_eMeshType == MeshType::ANIM ? m_pAnimModelCom->Get_MeshCount() : m_pNonAnimModelCom->Get_MeshCount();

    for (_uint i = 0; i < iMeshCount; ++i)
    {
        if (m_eMeshType == MeshType::ANIM)
        {
            // 텍스처 바인딩
            if (FAILED(m_pAnimModelCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
                return E_FAIL;

            if (FAILED(m_pAnimModelCom->Bind_BoneMatrices("g_BoneMatrices", i)))
                return E_FAIL;

            m_pAnimModelCom->Render(i, 2);
        }
        else
        {
            // 텍스처 바인딩
            if (FAILED(m_pNonAnimModelCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
                return E_FAIL;

            m_pNonAnimModelCom->Render(i, 1);
        }
    }

    return S_OK;
}

void Client::CWeapon::Change_Anim(_int AnimIndex)
{
    if (m_pAnimModelCom != nullptr)
        m_pAnimModelCom->Set_Animation(AnimIndex);
}

void Client::CWeapon::Play_Trail()
{
    m_pTrailEffect->PlayLoop();
}

void Client::CWeapon::Stop_Trail()
{
    m_pTrailEffect->Stop();
}

HRESULT Client::CWeapon::Add_Child()
{
    CTrailFX::INIT_DESC initDesc;
    initDesc.iMaxTrailRectNum = 30;
    //initDesc.trailTexKey = L"Mask66001";
    initDesc.trailTexKey = L"FxMb1Bruceloong1208";
    initDesc.vLocalUpperPos = Vector3(0.f, 2.5f,  0.f);
    initDesc.vLocalDownPos = Vector3(0.f, 5.5f,  0.f);
    initDesc.vColor = Vector4(0.55f, 0.54f, 0.66f, 1.f);
    m_pTrailEffect = CTrailFX::Create(m_pDevice, m_pContext, initDesc);
    m_pTrailEffect->Stop();
    if (nullptr == m_pTrailEffect)
        return E_FAIL;

    return S_OK;
}

HRESULT Client::CWeapon::Add_Component(LEVEL eLevel, const wstring& strModelKey, MeshType eMeshType)
{
    if (m_eMeshType == MeshType::ANIM)
    {
        auto pModelProp = m_pGameInstance.lock()->Get_AnimModel(eLevel, strModelKey);
        m_pBufferCom = m_pAnimModelCom = CSkeletalMeshCom::Create(m_pDevice, m_pContext, pModelProp);

        if (nullptr == m_pAnimModelCom)
            return E_FAIL;

        // 쉐이더 세팅
        if (FAILED(m_pAnimModelCom->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxAnimMesh"))))
            return E_FAIL;
    }
    else
    {
        auto pModelProp = m_pGameInstance.lock()->Get_NonAnimModel(eLevel, strModelKey);
        m_pBufferCom = m_pNonAnimModelCom = CStaticMeshCom::Create(m_pDevice, m_pContext, pModelProp);

        if (nullptr == m_pNonAnimModelCom)
            return E_FAIL;

        // 쉐이더 세팅
        if (FAILED(m_pNonAnimModelCom->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxMesh"))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT Client::CWeapon::Bind_ShaderResources()
{
    Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
    Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

    if (FAILED(m_pBufferCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pBufferCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pBufferCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CWeapon> Client::CWeapon::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eLevel, const wstring& strModelKey, shared_ptr<CBone> socketBone, MeshType eMeshType, _bool useTrail, const Vector3& vInitPos)
{
    shared_ptr<CWeapon> pInstance = make_shared<CWeapon>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(eLevel, strModelKey, socketBone, vInitPos, eMeshType, useTrail)))
    {
        MSG_BOX("Failed to Created : CWeapon");
        pInstance.reset();
    }

    return pInstance;
}
