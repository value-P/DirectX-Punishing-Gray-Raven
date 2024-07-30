#include "pch.h"
#include "Tool_Loader.h"
#include "Tool_LogoLevel.h"
#include "Tool_LoadingLevel.h"
#include "Tool_GamePlayLevel.h"

#include "Tool_BackGround.h"

CTool_LoadingLevel::CTool_LoadingLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevel)
	:CLevel(pDevice, pContext), m_eNextLevel(eNextLevel)
{
}

HRESULT CTool_LoadingLevel::Initialize()
{
	m_pLoader = CTool_Loader::Create(m_eNextLevel, m_pDevice, m_pContext);
	if (m_pLoader == nullptr)
		return E_FAIL;

	if (FAILED(Ready_Layer_Tool_BackGround(L"Layer_Tool_BackGround")))
		return E_FAIL;

	return S_OK;
}

void CTool_LoadingLevel::Tick(_float fTimeDelta)
{
	// 로딩 완료시
	if (m_pLoader->isFinished())
	{
		shared_ptr<CLevel> pLevel = nullptr;

		switch (m_eNextLevel)
		{
		case LEVEL_LOGO:
			pLevel = CTool_LogoLevel::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_GAMEPLAY:
			pLevel = CTool_GamePlayLevel::Create(m_pDevice, m_pContext);
			break;
		}

		if (pLevel == nullptr)
			return;

		if(FAILED(CGameInstance::GetInstance()->Open_Level(m_eNextLevel,pLevel)))
			return;
	}
}

HRESULT CTool_LoadingLevel::Render()
{
	if (nullptr == m_pLoader) return E_FAIL;

#ifdef _DEBUG
	m_pLoader->Output_LoadingText();
#endif

	return S_OK;
}

HRESULT CTool_LoadingLevel::Init_Layer_Tool_BackGround()
{
	return S_OK;
}

HRESULT CTool_LoadingLevel::Init_Layer_UI()
{
	return S_OK;
}

HRESULT Tool::CTool_LoadingLevel::Ready_Layer_Tool_BackGround(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance.lock()->Add_Object(LEVEL_LOADING, strLayerTag, CTool_BackGround::Create(m_pDevice, m_pContext, L"Loading0", Vector3(0.f,0.f,1.f)), &g_TimeScale)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CTool_LoadingLevel> CTool_LoadingLevel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevel)
{
	shared_ptr<CTool_LoadingLevel> pInstance = make_shared<CTool_LoadingLevel>(pDevice, pContext, eNextLevel);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CTool_LoadingLevel");
		pInstance.reset();
	}

	return pInstance;
}
