#include "pch.h"
#include "Level_Loading.h"
#include "Loader.h"

#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Boss.h"
#include "Level_Boss2.h"
#include "BackGround.h"

CLevel_Loading::CLevel_Loading(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevel)
	:CLevel(pDevice, pContext), m_eNextLevel(eNextLevel)
{
}

HRESULT CLevel_Loading::Initialize()
{
	m_pLoader = CLoader::Create(m_eNextLevel, m_pDevice, m_pContext);
	if (m_pLoader == nullptr)
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround", m_eNextLevel)))
		return E_FAIL;

	g_TimeScale_Monster = 1.f;

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	// 로딩 완료시
	if (m_pLoader->isFinished())
	{
		shared_ptr<CLevel> pLevel = nullptr;

		switch (m_eNextLevel)
		{
		case LEVEL_LOGO:
			pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_GAMEPLAY:
			pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_BOSS:
			pLevel = CLevel_Boss::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_BOSS2:
			pLevel = CLevel_Boss2::Create(m_pDevice, m_pContext);
			break;
		}

		if (pLevel == nullptr)
			return;

		if (FAILED(CGameInstance::GetInstance()->Open_Level(m_eNextLevel, pLevel)))
			return;

	}
}

HRESULT CLevel_Loading::Render()
{
	if (nullptr == m_pLoader) return E_FAIL;

#ifdef _DEBUG
	m_pLoader->Output_LoadingText();
#endif

	return S_OK;
}

HRESULT CLevel_Loading::Init_Layer_UI()
{
	return S_OK;
}

HRESULT Client::CLevel_Loading::Ready_Layer_BackGround(const wstring& strLayerTag, LEVEL eLevel)
{
	wstring strTexKey = L"BgFightLoading8";

	switch (eLevel)
	{
	case Client::LEVEL_GAMEPLAY:
		strTexKey = L"BgFightLoading8";
		break;
	case Client::LEVEL_BOSS:
		strTexKey = L"BgFightLoading21";
		break;
	case Client::LEVEL_BOSS2:
		strTexKey = L"BgFightLoading";
		break;
	default:
		break;
	}

	if (FAILED(m_pGameInstance.lock()->Add_Object(LEVEL_LOADING, strLayerTag, CBackGround::Create(m_pDevice, m_pContext, strTexKey, Vector3(0.f, 0.f, 0.f)), &g_TimeScale_Player)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CLevel_Loading> CLevel_Loading::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevel)
{
	shared_ptr<CLevel_Loading> pInstance = make_shared<CLevel_Loading>(pDevice, pContext, eNextLevel);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		pInstance.reset();
	}

	return pInstance;
}
