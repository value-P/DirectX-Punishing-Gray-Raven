#include "pch.h"
#include "Tool_LogoLevel.h"

#include "Tool_LoadingLevel.h"
#include "Tool_BackGround.h"

Tool::CTool_LogoLevel::CTool_LogoLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CTool_LogoLevel::Initialize()
{
	if (FAILED(Ready_Layer_Tool_BackGround(L"Layer_Tool_BackGround")))
		return E_FAIL;

	return S_OK;
}

void CTool_LogoLevel::Tick(_float fTimeDelta)
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(m_pGameInstance.lock()->Open_Level(LEVEL_LOADING, CTool_LoadingLevel::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;

		return;
	}
}

HRESULT CTool_LogoLevel::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif

	return S_OK;
}

HRESULT Tool::CTool_LogoLevel::Ready_Layer_Tool_BackGround(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance.lock()->Add_Object(LEVEL_LOGO, strLayerTag, CTool_BackGround::Create(m_pDevice, m_pContext, L"Logo", Vector3(0.f,0.f,0.f)), &g_TimeScale)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CTool_LogoLevel> CTool_LogoLevel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CTool_LogoLevel> pInstance = make_shared<CTool_LogoLevel>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CTool_LogoLevel");
		pInstance.reset();
	}

	return pInstance;
}
