#include "pch.h"
#include "Level_Logo.h"

#include "Level_Loading.h"
#include "BackGround.h"

Client::CLevel_Logo::CLevel_Logo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	m_pGameInstance.lock()->Play_BGM(L"LogoBgm",0.2f);

	return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(m_pGameInstance.lock()->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;

		return;
	}
}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif

	return S_OK;
}

HRESULT Client::CLevel_Logo::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance.lock()->Add_Object(LEVEL_LOGO, strLayerTag, CBackGround::Create(m_pDevice, m_pContext, L"Logo",Vector3(0.f,0.f,1.f)), &g_TimeScale_Player)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CLevel_Logo> CLevel_Logo::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CLevel_Logo> pInstance = make_shared<CLevel_Logo>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		pInstance.reset();
	}

	return pInstance;
}
