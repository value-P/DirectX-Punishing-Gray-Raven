#include "pch.h"
#include "SkillBtn.h"
#include "PlayableVera.h"
#include "Shader.h"
#include "Texture.h"
#include "PointProp.h"
#include "SkillBtnContainer.h"

CSkillBtn::CSkillBtn(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CUIBase(pDevice, pContext)
{
}

void Client::CSkillBtn::Set_Pos(Vector2 vPos)
{
    m_vPosition = vPos;
}

void CSkillBtn::Activate(SKILLCOLOR eColor,Vector2 vStartPos, Vector2 vDestPos)
{
    __super::Activate();

    m_vPosition = vStartPos;
    m_eCurrentColor = eColor;
    m_pTexture = m_pSkillTextures[m_eCurrentColor];
    m_vDestPos = vDestPos;
}

void CSkillBtn::InActivate()
{
    __super::InActivate();

    m_pFriendButtonList.clear();
    m_iSkillStack = 1;
}

void Client::CSkillBtn::Set_Player(shared_ptr<CPlayableVera> pPlayer)
{
    m_pPlayer = pPlayer;
}

void Client::CSkillBtn::Add_FriendBtn(shared_ptr<CSkillBtn> pBtn)
{
    m_pFriendButtonList.push_back(pBtn);
}

HRESULT CSkillBtn::Initialize(const INIT_DESC& initDesc, shared_ptr<CSkillBtnContainer> pContainer)
{
    if (nullptr == pContainer)
        return E_FAIL;

    m_pContainer = pContainer;

    if (FAILED(__super::Initialize(initDesc)))
        return E_FAIL;

    m_ProjMatrix = XMMatrixOrthographicLH(m_vViewPortSize.x, m_vViewPortSize.y, 0.f, 1.f);

    m_pShader = m_pGameInstance.lock()->Get_Shader(L"Shader_VtxPoint");
    if (nullptr == m_pShader)
        return E_FAIL;

    m_pSkillTextures[RED] = m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, L"FightSkillIconR3Weila11");
    if (nullptr == m_pSkillTextures[RED])
        return E_FAIL;
    m_pSkillTextures[BLUE] = m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, L"FightSkillIconR3Weila21");
    if (nullptr == m_pSkillTextures[BLUE])
        return E_FAIL;
    m_pSkillTextures[YELLOW] = m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, L"FightSkillIconR3Weila31");
    if (nullptr == m_pSkillTextures[YELLOW])
        return E_FAIL;

    return S_OK;
}

_int CSkillBtn::Tick(_float fTimeDelta)
{
    if (false == m_bActivate)
        return OBJ_ALIVE;

    __super::Tick(fTimeDelta);

    if (m_vPosition.x < m_vDestPos.x)
    {
        m_vPosition.x += 1500.f * fTimeDelta;
        if (m_vPosition.x >= m_vDestPos.x)
            m_vPosition.x = m_vDestPos.x;
    }

    m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_UI, shared_from_this());

    return OBJ_ALIVE;
}

HRESULT CSkillBtn::Render()
{
    Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    Vector3 vPos;
    vPos.x = m_vPosition.x - m_vViewPortSize.x * 0.5f;
    vPos.y = -m_vPosition.y + m_vViewPortSize.y * 0.5f;
    vPos.z = 0.f;

    WorldMatrix.Translation(vPos);

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_SRV("g_Texture", m_pTexture->GetSRV(0))))
        return E_FAIL;

    if (FAILED(m_pShader->Begin(0, 0)))
        return E_FAIL;

    m_pPointBuffer->Bind_Buffers();
    m_pPointBuffer->Render();

    return S_OK;
}

void CSkillBtn::OnClickEnter()
{
    if (m_vPosition.x != m_vDestPos.x)
        return;

    switch (m_eCurrentColor)
    {
    case Client::CSkillBtn::RED:
        m_pPlayer.lock()->UseSkill(CPlayableVera::RED, m_iSkillStack);
        break;
    case Client::CSkillBtn::BLUE:
        m_pPlayer.lock()->UseSkill(CPlayableVera::BLUE, m_iSkillStack);
        break;
    case Client::CSkillBtn::YELLOW:
        m_pPlayer.lock()->UseSkill(CPlayableVera::YELLOW, m_iSkillStack);
        break;
    default:
        return;
    }

    for (auto& pBtn : m_pFriendButtonList)
        pBtn.lock()->InActivate();

    InActivate();

    m_pContainer.lock()->CheckSkillChain();
}

shared_ptr<CSkillBtn> CSkillBtn::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc, shared_ptr<CSkillBtnContainer> pContainer)
{
    shared_ptr<CSkillBtn> pInstance = make_shared<CSkillBtn>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(initDesc, pContainer)))
    {
        MSG_BOX("Failed to Created : CSkillBtn");
        pInstance.reset();
    }

    return pInstance;
}
