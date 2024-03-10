#include "pch.h"
#include "Level_GamePlay.h"
#include "Camera_Combined.h"
#include "PlayableObject.h"
#include "PlayableVera.h"
#include "Level_Loading.h"
#include "MapObject.h"
#include "SkyBox.h"
#include "MonsterSpawner.h"
#include "ActionCamera.h"
#include "EventTrigger.h"
#include "DistortEffect.h"
#include "DistortionPointRect.h"
#include "TextUI.h"

Client::CLevel_GamePlay::CLevel_GamePlay(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT Client::CLevel_GamePlay::Initialize()
{
	m_pGameInstance.lock()->UseFog(true);

	m_pGameInstance.lock()->Play_BGM(L"Stage1Bgm", 0.1f);

	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;
	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;
	if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
		return E_FAIL;
	if (FAILED(Ready_Layer_Monster(L"Layer_Monster")))
		return E_FAIL;
	if (FAILED(Ready_Layer_SkyBox(L"Layer_SkyBox")))
		return E_FAIL;

	m_pPlayableChar->PlayBornSound();

	return S_OK;
}

void Client::CLevel_GamePlay::Tick(_float fTimeDelta)
{
	if (m_pGameInstance.lock()->GetKeyDown(DIK_F4))
	{
		//m_pGameInstance.lock()->ScreenBlurActivate(1.f);

		m_pGameInstance.lock()->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BOSS));
		return;
	}

	if (m_pSpawner[0]->AllMonseterCleared() && !bSpawnerUsed[0])
	{
		m_pSpawner[1]->Set_Activate(true);
		bSpawnerUsed[0] = true;
	}
		
	if (m_pSpawner[1]->AllMonseterCleared() && !bSpawnerUsed[1])
	{
		m_pSpawner[2]->Set_Activate(true);
		bSpawnerUsed[1] = true;
	}

	if (m_pSpawner[2]->AllMonseterCleared() && !bSpawnerUsed[2])
	{
		m_pSceneChanger->Set_Activate(true);
		bSpawnerUsed[1] = true;
	}

}

HRESULT Client::CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("게임플레이 레벨입니다."));
#endif

	return S_OK;
}

HRESULT Client::CLevel_GamePlay::Ready_Layer_Camera(const wstring& strLayerTag)
{
	if (m_pPlayableChar == nullptr)
		return E_FAIL;

	// 플레이어 combined메인카메라
	Vector3 vLook = m_pPlayableChar->Get_Transform()->Get_Look();
	
	shared_ptr<CCamera_Combined> pCam = CCamera_Combined::Create(XMConvertToRadians(60.f), 0.1f, 1000.f, (_float)g_iWinSizeX / (_float)g_iWinSizeY, m_pPlayableChar, -vLook * 6.f);
	if (FAILED(m_pGameInstance.lock()->Add_Camera(LEVEL_GAMEPLAY, L"Player_Combined", pCam)))
		return E_FAIL;

	m_pPlayableChar->Set_CombinedCamera(pCam);

	// 입장 연출 카메라
	{
		CActionCamera::INIT_DESC actionCamDesc;
		actionCamDesc.keyFrames.resize(3);

		actionCamDesc.keyFrames[0].vPosition = Vector3(-18.f,1.4f,-12.5f);
		actionCamDesc.keyFrames[0].vLookPosition = Vector3(-16.f, 1.1f, -11.f);
		actionCamDesc.keyFrames[0].fKeyframeTime = 0.f;
		actionCamDesc.keyFrames[0].bMoveLerp = true;

		actionCamDesc.keyFrames[1].vPosition = Vector3(-18.5f, 1.4f, -11.5f);
		actionCamDesc.keyFrames[1].vLookPosition = Vector3(-16.f, 1.1f, -11.f);
		actionCamDesc.keyFrames[1].fKeyframeTime = 1.5f;
		actionCamDesc.keyFrames[1].bMoveLerp = true;

		actionCamDesc.keyFrames[2].vPosition = Vector3(-19.f, 1.4f, -10.5f);
		actionCamDesc.keyFrames[2].vLookPosition = Vector3(-16.f, 1.1f, -11.f);
		actionCamDesc.keyFrames[2].fKeyframeTime = 3.f;
		actionCamDesc.keyFrames[2].bMoveLerp = true;
		
		shared_ptr<CActionCamera> pActionCam = CActionCamera::Create(m_pDevice, m_pContext, XMConvertToRadians(60.f), 0.1f, 1000.f, (_float)g_iWinSizeX / (_float)g_iWinSizeY, actionCamDesc, LEVEL_GAMEPLAY, L"Player_Combined");
		if (FAILED(m_pGameInstance.lock()->Add_Camera(LEVEL_GAMEPLAY, L"Player_Born", pActionCam)))
			return E_FAIL;

		m_pGameInstance.lock()->Set_MainCamera(LEVEL_GAMEPLAY, L"Player_Born");
	}

	// 궁 연출 카메라
	{
		CActionCamera::INIT_DESC actionCamDesc;
		actionCamDesc.pRefObject = m_pPlayableChar;

		actionCamDesc.keyFrames.resize(8);

		actionCamDesc.keyFrames[0].vPosition = Vector3(-0.7f,1.36f,1.3f);
		actionCamDesc.keyFrames[0].vLookPosition = Vector3(0.f, 1.4f, 0.f);
		actionCamDesc.keyFrames[0].fKeyframeTime = 0.f;
		actionCamDesc.keyFrames[0].bMoveLerp = true;

		actionCamDesc.keyFrames[1].vPosition = Vector3(-1.3f,1.46f,0.3f);
		actionCamDesc.keyFrames[1].vLookPosition = Vector3(0.f, 1.4f, 0.f);
		actionCamDesc.keyFrames[1].fKeyframeTime = 0.3f;
		actionCamDesc.keyFrames[1].bMoveLerp = true;

		actionCamDesc.keyFrames[2].vPosition = Vector3(-0.5f,1.5f,0.76f);
		actionCamDesc.keyFrames[2].vLookPosition = Vector3(0.f, 1.4f, 0.f);
		actionCamDesc.keyFrames[2].fKeyframeTime = 0.6f;
		actionCamDesc.keyFrames[2].bMoveLerp = true;
		
		actionCamDesc.keyFrames[3].vPosition = Vector3(-0.2f,1.52f,0.75f);
		actionCamDesc.keyFrames[3].vLookPosition = Vector3(0.f, 1.4f, 0.f);
		actionCamDesc.keyFrames[3].fKeyframeTime = 0.75f;
		actionCamDesc.keyFrames[3].bMoveLerp = true;
		
		actionCamDesc.keyFrames[4].vPosition = Vector3(0.f,1.54f,0.9f);
		actionCamDesc.keyFrames[4].vLookPosition = Vector3(0.f, 1.4f, 0.f);
		actionCamDesc.keyFrames[4].fKeyframeTime = 0.9f;
		actionCamDesc.keyFrames[4].bMoveLerp = true;
		
		actionCamDesc.keyFrames[5].vPosition = Vector3(0.f,1.54f,0.9f);
		actionCamDesc.keyFrames[5].vLookPosition = Vector3(0.f, 1.4f, 0.f);
		actionCamDesc.keyFrames[5].fKeyframeTime = 1.12f;
		actionCamDesc.keyFrames[5].bMoveLerp = true;
		
		actionCamDesc.keyFrames[6].vPosition = Vector3(0.f,1.7f,7.7f);
		actionCamDesc.keyFrames[6].vLookPosition = Vector3(0.f, 1.4f, 0.f);
		actionCamDesc.keyFrames[6].fKeyframeTime = 1.23f;
		actionCamDesc.keyFrames[6].bMoveLerp = true;

		actionCamDesc.keyFrames[7].vPosition = Vector3(0.f,1.7f,7.7f);
		actionCamDesc.keyFrames[7].vLookPosition = Vector3(0.f, 1.4f, 0.f);
		actionCamDesc.keyFrames[7].fKeyframeTime = 2.3f;
		actionCamDesc.keyFrames[7].bMoveLerp = true;

		shared_ptr<CActionCamera> pActionCam = CActionCamera::Create(m_pDevice, m_pContext, XMConvertToRadians(60.f), 0.1f, 1000.f, (_float)g_iWinSizeX / (_float)g_iWinSizeY, actionCamDesc, LEVEL_GAMEPLAY, L"Player_Combined");
		if (FAILED(m_pGameInstance.lock()->Add_Camera(LEVEL_GAMEPLAY, L"Player_Ultimate", pActionCam)))
			return E_FAIL;
	}

	// 퇴장 연출 카메라
	{
		CActionCamera::INIT_DESC actionCamDesc;
		actionCamDesc.pRefObject = m_pPlayableChar;

		actionCamDesc.keyFrames.resize(2);

		actionCamDesc.keyFrames[0].vPosition = Vector3(-0.15f,1.4f,0.7f);
		actionCamDesc.keyFrames[0].vLookPosition = Vector3(0.f,1.4f,0.f);
		actionCamDesc.keyFrames[0].fKeyframeTime = 0.f;
		actionCamDesc.keyFrames[0].bMoveLerp = true;

		actionCamDesc.keyFrames[1].vPosition = Vector3(-0.2f,1.3f,0.9f);
		actionCamDesc.keyFrames[1].vLookPosition = Vector3(0.f, 1.4f, 0.f);
		actionCamDesc.keyFrames[1].fKeyframeTime = 5.2f;
		actionCamDesc.keyFrames[1].bMoveLerp = true;

		shared_ptr<CActionCamera> pActionCam = CActionCamera::Create(m_pDevice, m_pContext, XMConvertToRadians(60.f), 0.1f, 1000.f, (_float)g_iWinSizeX / (_float)g_iWinSizeY, actionCamDesc, LEVEL_GAMEPLAY, L"Player_Combined", LEVEL_BOSS, true);
		if (FAILED(m_pGameInstance.lock()->Add_Camera(LEVEL_GAMEPLAY, L"Player_Win", pActionCam)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT Client::CLevel_GamePlay::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	// 맵 생성
	CMapObject::INIT_DESC mapDesc;
	mapDesc.eLevel = LEVEL_GAMEPLAY;
	mapDesc.strModelKey = L"Scene00002c";
	mapDesc.vInitPos = Vector3(0.f,0.f,0.f);
	shared_ptr<CMapObject> pMap = CMapObject::Create(m_pDevice, m_pContext, mapDesc);
	if (FAILED(m_pGameInstance.lock()->Add_Object(LEVEL_GAMEPLAY, strLayerTag, pMap, &g_TimeScale_Player)))
		return E_FAIL;

	shared_ptr<CNavMeshProp> pNavMesh = m_pGameInstance.lock()->Get_NavMesh(LEVEL_GAMEPLAY, L"Scene00002c");
	if (FAILED(m_pGameInstance.lock()->Set_NavMesh(LEVEL_GAMEPLAY,pNavMesh, pMap)))
		return E_FAIL;

	// 씬 전환 트리거 생성
	CEventTrigger::INIT_DESC eventTriggerDesc;
	eventTriggerDesc.vInitPos = Vector3(-113.f, 0.f, -13.f);
	eventTriggerDesc.eLevel = LEVEL_GAMEPLAY;
	m_pSceneChanger = CEventTrigger::Create(m_pDevice, m_pContext, eventTriggerDesc);
	m_pGameInstance.lock()->Add_Object(LEVEL_GAMEPLAY, L"Layer_EventTrigger", m_pSceneChanger, &g_TimeScale_Player);

	// 임무 UI 배경 + font
	CTextUI::INIT_DESC bgDesc;
	bgDesc.vPos = Vector2(40.f, 150.f);
	bgDesc.vSize = Vector2(320.f, 100.f);
	//bgDesc.strTexKey = L"DormTabBg";
	bgDesc.strTexKey = L"CommonBtnTong01Xiao";
	bgDesc.fFontSize = 0.4f;
	bgDesc.strText = L"목표까지 전진하라";
	m_pBgText = CTextUI::Create(m_pDevice, m_pContext, bgDesc);
	m_pBgText->Activate();
	m_pGameInstance.lock()->Add_UI(LEVEL_GAMEPLAY, m_pBgText);

	return S_OK;
}

HRESULT Client::CLevel_GamePlay::Ready_Layer_Player(const wstring& strLayerTag)
{
	CPlayableVera::INIT_DESC playerDesc;
	playerDesc.ePropLevel = LEVEL_STATIC;
	playerDesc.eCreateLevel = LEVEL_GAMEPLAY;
	playerDesc.strModelKey = L"R3WeilaMd010021 (merge)";
	playerDesc.vInitPos = Vector3(-16.f, 0.f, -11.f);
	playerDesc.iHp = 500;
	playerDesc.iAttackPower = 15;
	m_pPlayableChar = CPlayableVera::Create(m_pDevice, m_pContext, playerDesc);
	m_pPlayableChar->Get_Transform()->Rotation(Vector4(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance.lock()->Add_Object(LEVEL_GAMEPLAY, strLayerTag, m_pPlayableChar, &g_TimeScale_Player)))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::CLevel_GamePlay::Ready_Layer_Monster(const wstring& strLayerTag)
{
	// ***********************
	// 스포너 1
	// ***********************

	CMonsterSpawner::INIT_DESC spawnerDesc0;
	spawnerDesc0.eCreateLevel = LEVEL_GAMEPLAY;
	spawnerDesc0.pTargetChar = m_pPlayableChar;
	spawnerDesc0.vInitPos = Vector3(-24.6f, -0.01f, -13.f);

	spawnerDesc0.vSawManPositions.resize(2);
	spawnerDesc0.vSawManPositions[0] = Vector3(-26.6f, -0.01f,-19.25f);
	spawnerDesc0.vSawManPositions[1] = Vector3(-26.6f,-0.01f,-8.4f);

	m_pSpawner[0] = CMonsterSpawner::Create(m_pDevice, m_pContext, spawnerDesc0);
	if (nullptr == m_pSpawner[0])
	{
		MSG_BOX("CLevel_GamePlay::Ready_Layer_Monster");
		return E_FAIL;
	}
	m_pGameInstance.lock()->Add_Object(LEVEL_GAMEPLAY, L"Layer_Spawner", m_pSpawner[0], &g_TimeScale_Monster);

	m_pSpawner[0]->Set_Activate(true);

	// ***********************
	// 스포너 2
	// ***********************
	CMonsterSpawner::INIT_DESC spawnerDesc1;
	spawnerDesc1.eCreateLevel = LEVEL_GAMEPLAY;
	spawnerDesc1.pTargetChar = m_pPlayableChar;
	spawnerDesc1.vInitPos = Vector3(-48.f, -0.01f, -13.f);

	spawnerDesc1.vRobotPositions.resize(2);
	spawnerDesc1.vRobotPositions[0] = Vector3(-49.f,-0.01f,-17.f);
	spawnerDesc1.vRobotPositions[1] = Vector3(-49.5f,-0.01f,-12.5f);

	m_pSpawner[1] = CMonsterSpawner::Create(m_pDevice, m_pContext, spawnerDesc1);
	if (nullptr == m_pSpawner[1])
	{
		MSG_BOX("CLevel_GamePlay::Ready_Layer_Monster");
		return E_FAIL;
	}
	m_pGameInstance.lock()->Add_Object(LEVEL_GAMEPLAY, L"Layer_Spawner", m_pSpawner[1], &g_TimeScale_Monster);

	// ***********************
	// 스포너 3
	// ***********************
	CMonsterSpawner::INIT_DESC spawnerDesc2;
	spawnerDesc2.eCreateLevel = LEVEL_GAMEPLAY;
	spawnerDesc2.pTargetChar = m_pPlayableChar;
	spawnerDesc2.vInitPos = Vector3(-79.f, 0.f,-13.f);

	spawnerDesc2.vSawManPositions.resize(2);
	spawnerDesc2.vSawManPositions[0] = Vector3(-84.f,0.f,-18.f);
	spawnerDesc2.vSawManPositions[1] = Vector3(-84.f, 0.f, -10.f);

	spawnerDesc2.vRobotPositions.resize(1);
	spawnerDesc2.vRobotPositions[0] = Vector3(-87.f, 0.f, -14.f);

	m_pSpawner[2] = CMonsterSpawner::Create(m_pDevice, m_pContext, spawnerDesc2);
	if (nullptr == m_pSpawner[2])
	{
		MSG_BOX("CLevel_GamePlay::Ready_Layer_Monster");
		return E_FAIL;
	}
	m_pGameInstance.lock()->Add_Object(LEVEL_GAMEPLAY, L"Layer_Spawner", m_pSpawner[2], &g_TimeScale_Monster);

	return S_OK;
}

HRESULT Client::CLevel_GamePlay::Ready_Layer_SkyBox(const wstring& strLayerTag)
{
	CSkyBox::INIT_DESC skyDesc;
	skyDesc.vInitPos = m_pGameInstance.lock()->Get_CamPos();
	skyDesc.strTextureKey = L"GraySky";

	shared_ptr<CSkyBox> pSkyBox = CSkyBox::Create(m_pDevice, m_pContext, skyDesc);
	if (FAILED(m_pGameInstance.lock()->Add_Object(LEVEL_GAMEPLAY, strLayerTag, pSkyBox, &g_TimeScale_Player)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CLevel_GamePlay> Client::CLevel_GamePlay::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CLevel_GamePlay> pInstance = make_shared<CLevel_GamePlay>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		pInstance.reset();
	}

	return pInstance;
}
