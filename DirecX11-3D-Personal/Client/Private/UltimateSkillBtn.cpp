#include "pch.h"
#include "UltimateSkillBtn.h"
#include "Shader.h"
#include "Texture.h"
#include "PointProp.h"
#include "PlayableVera.h"

Client::CUltimateSkillBtn::CUltimateSkillBtn(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CUIBase(pDevice, pContext)
{
}

void Client::CUltimateSkillBtn::Set_Player(shared_ptr<CPlayableVera> pPlayer)
{
    m_pPlayer = pPlayer;
}

HRESULT Client::CUltimateSkillBtn::Initialize(const INIT_DESC& initDesc)
{
    if (FAILED(__super::Initialize(initDesc)))
        return E_FAIL;

    m_ProjMatrix = XMMatrixOrthographicLH(m_vViewPortSize.x, m_vViewPortSize.y, 0.f, 1.f);

    m_pShader = m_pGameInstance.lock()->Get_Shader(L"Shader_VtxPoint");
    if (nullptr == m_pShader)
        return E_FAIL;

    m_pTexture = m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, initDesc.strTexKey);
    if (nullptr == m_pTexture)
        return E_FAIL;
    
    m_pBgTexture = m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, L"FightBtnBg4");
    if (nullptr == m_pBgTexture)
        return E_FAIL;

    return S_OK;
}

_int Client::CUltimateSkillBtn::Tick(_float fTimeDelta)
{
    if (false == m_bActivate)
        return OBJ_ALIVE;

    __super::Tick(fTimeDelta);

    m_fBgRotationAcc += m_fBgRotationSpeed * fTimeDelta;
    if (m_fBgRotationAcc >= 360.f)
        m_fBgRotationAcc -= 360.f;

    m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_UI, shared_from_this());

    return OBJ_ALIVE;
}

HRESULT Client::CUltimateSkillBtn::Render()
{
    // Å×µÎ¸® ·»´õ
    Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    Vector3 vPos;
    vPos.x = m_vPosition.x - m_vViewPortSize.x * 0.5f;
    vPos.y = -m_vPosition.y + m_vViewPortSize.y * 0.5f;
    vPos.z = 0.f;

    WorldMatrix.Translation(vPos);

    Matrix BgMatrix = WorldMatrix;
    BgMatrix._11 += 20.f;
    BgMatrix._22 += 20.f;

    Matrix matRot = XMMatrixRotationZ(XMConvertToRadians(m_fBgRotationAcc));
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &BgMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_RotationMatrix", &matRot)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_SRV("g_Texture", m_pBgTexture->GetSRV(0))))
        return E_FAIL;

    if (FAILED(m_pShader->Begin(2, 0)))
        return E_FAIL;

    m_pPointBuffer->Bind_Buffers();
    m_pPointBuffer->Render();

    // ¾ÆÀÌÄÜ ·»´õ
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_SRV("g_Texture", m_pTexture->GetSRV(0))))
        return E_FAIL;

    if (FAILED(m_pShader->Begin(0, 0)))
        return E_FAIL;

    m_pPointBuffer->Bind_Buffers();
    m_pPointBuffer->Render();

    return S_OK;
}

void Client::CUltimateSkillBtn::OnClickEnter()
{
    if (m_pPlayer.lock()->UseUltimateSkill())
        InActivate();
}

shared_ptr<CUltimateSkillBtn> Client::CUltimateSkillBtn::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc)
{
    shared_ptr<CUltimateSkillBtn> pInstance = make_shared<CUltimateSkillBtn>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(initDesc)))
    {
        MSG_BOX("Failed to Created : CUltimateSkillBtn");
        pInstance.reset();
    }

    return pInstance;
}
