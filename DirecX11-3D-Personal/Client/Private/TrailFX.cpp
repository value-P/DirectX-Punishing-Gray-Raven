#include "pch.h"
#include "TrailFX.h"

Client::CTrailFX::CTrailFX(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CFX(pDevice, pContext)
{
}

HRESULT Client::CTrailFX::Initialize(const INIT_DESC& InitDesc)
{
    m_vUpperPos = InitDesc.vLocalUpperPos;
    m_vDownPos = InitDesc.vLocalDownPos;
    m_vColor = InitDesc.vColor;

    m_pTrailCom = CTrailCom::Create(m_pDevice, m_pContext, InitDesc.iMaxTrailRectNum);
    if (nullptr == m_pTrailCom)
        return E_FAIL;
    
    if(FAILED(m_pTrailCom->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxPosTex"))))
        return E_FAIL;
    if(FAILED(m_pTrailCom->Setting_Texture(m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, InitDesc.trailTexKey))))
        return E_FAIL;

    return S_OK;
}

_int Client::CTrailFX::Tick(const Matrix& matWorld)
{
    /*if (!m_bActivate)
        return OBJ_ALIVE;*/

    m_pTrailCom->Tick(XMVector3TransformCoord(m_vUpperPos,matWorld), XMVector3TransformCoord(m_vDownPos, matWorld));

    if (m_bActivate)
        m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, shared_from_this());

    return OBJ_ALIVE;
}

HRESULT Client::CTrailFX::Render()
{
    Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
    Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

    if (FAILED(m_pTrailCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pTrailCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pTrailCom->Bind_SRV("g_Texture")))
        return E_FAIL;
    if (FAILED(m_pTrailCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(Vector4))))
        return E_FAIL;

    m_pTrailCom->Render(1, 0);

    return S_OK;
}

shared_ptr<CTrailFX> Client::CTrailFX::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc)
{
    shared_ptr<CTrailFX> pInstance = make_shared<CTrailFX>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(InitDesc)))
    {
        MSG_BOX("Failed to Created : CTrailFX");
        pInstance.reset();
    }

    return pInstance;
}
