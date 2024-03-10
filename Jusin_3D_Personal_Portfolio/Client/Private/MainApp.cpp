#include "pch.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CMainApp::CMainApp()
	:m_pGameInstance(CGameInstance::GetInstance())
{
}

CMainApp::~CMainApp()
{
	Release();
}

HRESULT CMainApp::Initialize()
{
	GRAPHIC_DESC	GraphicDesc = {};

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.isWindowed = true;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;
	GraphicDesc.iUpScalingSizeX = 8192;
	GraphicDesc.iUpScalingSizeY = 4608;

	vector<_uint> CollisionMatrix;
	Make_Layer_Collision_Matrix(CollisionMatrix);

	// 기초 초기화
	if(FAILED(m_pGameInstance.lock()->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, m_pDevice, m_pContext, CollisionMatrix)))
		return E_FAIL;

	// 시작 레벨 할당
	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	// 기본 폰트 추가
	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/Punishing.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Font_Punishing"), TEXT("../Bin/Resources/Fonts/PunishingDefault.spritefont"))))
		return E_FAIL;

	// BGM추가
	m_pGameInstance.lock()->LoadSound(L"../Bin/Resources/Sounds/Bgm");

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Tick_Engine(fTimeDelta);

#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif // _DEBUG
}

HRESULT CMainApp::Render()
{
	if(FAILED(m_pGameInstance.lock()->Begin_Draw(Color(0.f, 0.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Draw()))
		return E_FAIL;

#ifdef _DEBUG
	++m_iNumRender;

	if (m_fTimeAcc >= 1.f)
	{
		m_strFPS = L"FPS:" + to_wstring(m_iNumRender);

		m_iNumRender = 0;
		m_fTimeAcc = 0.f;
	}

	//if (FAILED(m_pGameInstance.lock()->Render_Font(TEXT("Font_Default"), m_strFPS, Vector2(35.f, 25.f), Vector4(1.f, 0.f, 0.f, 1.f), 0.f,0.5f)))
	//	return E_FAIL;
#endif // _DEBUG

	if (FAILED(m_pGameInstance.lock()->End_Draw()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eStartLevel)
{
	if (eStartLevel == LEVEL_LOADING)
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Load_Single_Textures_FromFile(L"../Bin/Resources/Textures/SingleTex", LEVEL_STATIC)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_VtxPosTex", L"../Bin/ShaderFiles/Shader_VtxPosTex.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_VtxPoint", L"../Bin/ShaderFiles/Shader_VtxPoint.hlsl", VTXPOINT::Elements, VTXPOINT::iNumElements)))
		return E_FAIL;

	return m_pGameInstance.lock()->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevel));
}

void Client::CMainApp::Make_Layer_Collision_Matrix(vector<_uint>& collisionMatrix)
{
	collisionMatrix.resize(Col_End);

	collisionMatrix[Col_Player]				= 1 << Col_Monster | 1 << Col_MonsterAttack | 1 << Col_Environment | 1 << Col_MonsterHardAttack;
	collisionMatrix[Col_Monster]			= 1 << Col_Player | 1 << Col_Monster | 1 << Col_PlayerAttack | 1 << Col_PlayerHardAttack | 1 << Col_MonsterParry;
	collisionMatrix[Col_PlayerAttack]		= 1 << Col_Monster;
	collisionMatrix[Col_PlayerHardAttack]	= 1 << Col_Monster;
	collisionMatrix[Col_MonsterAttack]		= 1 << Col_Player | 1 << Col_PlayerDodge;
	collisionMatrix[Col_MonsterHardAttack]	= 1 << Col_Player;
	collisionMatrix[Col_Environment]		= 1 << Col_Player;
	collisionMatrix[Col_MonsterParry]		= 1 << Col_PlayerAttack;
	collisionMatrix[Col_PlayerDodge]		= 1 << Col_MonsterAttack;
}

shared_ptr<CMainApp> CMainApp::Create()
{
	shared_ptr<CMainApp> spInstance = make_shared<CMainApp>();

	if (FAILED(spInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		spInstance.reset();
	}

	return spInstance;
}

void CMainApp::Release()
{
	m_pDevice.Reset();
	m_pContext.Reset();
	m_pGameInstance.reset();
	CGameInstance::DestroyInstance();
}
