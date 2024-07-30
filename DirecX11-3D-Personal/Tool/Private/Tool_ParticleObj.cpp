#include "pch.h"
#include "Tool_ParticleObj.h"

Tool::CTool_ParticleObj::CTool_ParticleObj(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice, pContext)
{
}

void Tool::CTool_ParticleObj::PlayOnce()
{
    m_pParticleSystem->PlayOnce();
}

void Tool::CTool_ParticleObj::PlayLoop()
{
    m_pParticleSystem->PlayLoop();
}

void Tool::CTool_ParticleObj::Stop()
{
    m_pParticleSystem->Stop();
}

HRESULT Tool::CTool_ParticleObj::Initialize(_uint iNumInstance, const Vector3& vInitPos, LEVEL eLevel, const wstring& strTextureKey, const CPointParticle::INSTANCE_DESC& initData)
{
    if (FAILED(__super::Initialize(vInitPos)))
        return E_FAIL;

    if (FAILED(Add_Component(iNumInstance, initData)))
        return E_FAIL;

    if (FAILED(Set_Texture(LEVEL_STATIC, strTextureKey)))
        return E_FAIL;

    return S_OK;
}

void Tool::CTool_ParticleObj::Priority_Tick(_float fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);
}

_int Tool::CTool_ParticleObj::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (!m_pParticleSystem->isStopped())
        m_pParticleSystem->Tick(fTimeDelta);

    return OBJ_ALIVE;
}

void Tool::CTool_ParticleObj::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    if (!m_pParticleSystem->isStopped())
        m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_BLEND, shared_from_this());
}

HRESULT Tool::CTool_ParticleObj::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    auto moveOption = m_pParticleSystem->Get_ParticleOption();

    if (moveOption == CParticleSystem::Drop)
        m_pParticleSystem->Render(0);
    else if (moveOption == CParticleSystem::Spread)
        m_pParticleSystem->Render(1);

    return S_OK;
}

void Tool::CTool_ParticleObj::Change_Option(CParticleSystem::PARTICLE_OPTION eTickOption)
{
    m_pParticleSystem->Change_Option(eTickOption);
}

void Tool::CTool_ParticleObj::Change_InstanceData(const CPointParticle::INSTANCE_DESC& InstanceData)
{
    m_pParticleSystem->Change_ParticleData(InstanceData);
}

HRESULT Tool::CTool_ParticleObj::Set_Texture(LEVEL eLevel, const wstring& strTextureKey)
{
    shared_ptr<CTexture> pTex = m_pGameInstance.lock()->Get_Texture(eLevel, strTextureKey);
    if (pTex == nullptr)
    {
        MSG_BOX("존재하지 않는 텍스처입니다");
        return E_FAIL;
    }

    m_pParticleSystem->Setting_Texture(SpriteTexType::TEX_DIFFUSE, pTex);

    return S_OK;
}

HRESULT Tool::CTool_ParticleObj::Set_Texture(shared_ptr<CTexture> pTexture)
{
    if (nullptr == pTexture)
        return E_FAIL;

    m_pParticleSystem->Setting_Texture(SpriteTexType::TEX_DIFFUSE, pTexture);

    return S_OK;
}

HRESULT Tool::CTool_ParticleObj::Add_Component(_uint iNumInstance, const CPointParticle::INSTANCE_DESC& initData)
{
    m_pParticleSystem = CParticleSystem::Create(m_pDevice, m_pContext, iNumInstance, initData);
    if (nullptr == m_pParticleSystem)
        return E_FAIL;

    // 쉐이더 세팅
    m_pParticleSystem->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxPointInstance"));
    //m_pParticleSystem->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxRectInstance"));

    return S_OK;
}

HRESULT Tool::CTool_ParticleObj::Bind_ShaderResources()
{
    Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
    Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();
    Vector4 vCamPos = XMVectorSetW(m_pGameInstance.lock()->Get_CamPos(),1.f);

    if(FAILED(m_pParticleSystem->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
        return E_FAIL;
    if(FAILED(m_pParticleSystem->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;
    if(FAILED(m_pParticleSystem->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pParticleSystem->Bind_RawValue("g_vCamPosition", &vCamPos, sizeof(Vector4))))
        return E_FAIL;
    if (FAILED(m_pParticleSystem->Bind_SRV("g_Texture", SpriteTexType::TEX_DIFFUSE, 0)))
        return E_FAIL;

    Vector4 vPivotPos = XMVectorSetW(m_pTransformCom->Get_Pos() + m_pParticleSystem->Get_PivotPos(),1.f);
    if (FAILED(m_pParticleSystem->Bind_RawValue("g_vPivotPos", &vPivotPos, sizeof(Vector4))))
        return E_FAIL;

    return S_OK; 
}

shared_ptr<CTool_ParticleObj> Tool::CTool_ParticleObj::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumInstance, const Vector3& vInitPos, LEVEL eLevel, const wstring& strTextureKey, const CPointParticle::INSTANCE_DESC& initData)
{
    shared_ptr<CTool_ParticleObj> pInstance = make_shared<CTool_ParticleObj>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(iNumInstance, vInitPos, eLevel, strTextureKey, initData)))
    {
        MSG_BOX("Failed to Created : CTool_ParticleObj");
        pInstance.reset();
    }

    return pInstance;
}
