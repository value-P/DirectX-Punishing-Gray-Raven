#include "pch.h"
#include "FontUI.h"

Client::CFontUI::CFontUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CUIBase(pDevice, pContext)
{
}

HRESULT Client::CFontUI::Initialize(const INIT_DESC& initDesc)
{
	m_strFontTag = initDesc.strFontTag;
	m_vPosition = initDesc.vPosition;
	m_vColor = initDesc.vColor;
	m_fScale = initDesc.fScale;

	return S_OK;
}

_int Client::CFontUI::Tick(_float fTimeDelta)
{
	if (false == m_bActivate)
		return OBJ_ALIVE;

	__super::Tick(fTimeDelta);

	m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_UI, shared_from_this());

	return OBJ_ALIVE;
}

HRESULT Client::CFontUI::Render()
{
	return m_pGameInstance.lock()->Render_Font(m_strFontTag, m_strText, m_vPosition, m_vColor, 0.f, m_fScale);
}


shared_ptr<CFontUI> Client::CFontUI::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc)
{
	shared_ptr<CFontUI> pInstance = make_shared<CFontUI>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(initDesc)))
	{
		MSG_BOX("Failed to Created : CFontUI");
		pInstance.reset();
	}

	return pInstance;
}
