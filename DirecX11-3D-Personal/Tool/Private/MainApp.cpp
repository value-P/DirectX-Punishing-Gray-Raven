#include "pch.h"
#include "MainApp.h"
#include "GameInstance.h"

#include "Tool_LoadingLevel.h"
#include "SampleUI.h"

Tool::CMainApp::CMainApp()
	:m_pGameInstance(CGameInstance::GetInstance())
{
}

Tool::CMainApp::~CMainApp()
{
	Release();
}

HRESULT Tool::CMainApp::Initialize()
{
	GRAPHIC_DESC GraphicDesc = {};

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;
	GraphicDesc.isWindowed = true;
	GraphicDesc.iUpScalingSizeX = 8192;
	GraphicDesc.iUpScalingSizeY = 4608;

	vector<_uint> CollisionMatrix;

	// 기초 초기화
	if (FAILED(m_pGameInstance.lock()->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, m_pDevice, m_pContext, CollisionMatrix, ImGui::CreateContext())))
		return E_FAIL;

	if (FAILED(Open_Level(Tool::LEVEL_GAMEPLAY)))
		return E_FAIL;

	return S_OK;
}

void Tool::CMainApp::Tick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Tick_Engine(fTimeDelta);
}

HRESULT Tool::CMainApp::Render()
{
	if (FAILED(m_pGameInstance.lock()->Begin_Draw(Vector4(0.f, 0.5f, 0.5f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Draw()))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->End_Draw()))
		return E_FAIL;

	return S_OK;
}

HRESULT Tool::CMainApp::Open_Level(LEVEL eStartLevel)
{
	if (eStartLevel == LEVEL_LOADING)
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Load_Single_Textures_FromFile(L"../Bin/Resources/Textures/SingleTex", LEVEL_STATIC)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_VtxPosTex", L"../Bin/ShaderFiles/Shader_VtxPosTex.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements)))
		return E_FAIL;

	return m_pGameInstance.lock()->Open_Level(LEVEL_LOADING, CTool_LoadingLevel::Create(m_pDevice, m_pContext, eStartLevel));
}

shared_ptr<CMainApp> Tool::CMainApp::Create()
{
	shared_ptr<CMainApp> spInstance = make_shared<CMainApp>();

	if (FAILED(spInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		spInstance.reset();
	}

	return spInstance;
}

void Tool::CMainApp::Release()
{
	CGameInstance::DestroyInstance();
}
