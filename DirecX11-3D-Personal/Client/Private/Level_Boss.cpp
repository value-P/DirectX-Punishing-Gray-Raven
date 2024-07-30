#include "pch.h"
#include "Level_Boss.h"
#include "Camera_Combined.h"
#include "PlayableObject.h"
#include "PlayableVera.h"
#include "Boss_Pushin.h"
#include "Boss_StarKnight.h"
#include "MapObject.h"
#include "Level_Loading.h"
#include "ActionCamera.h"

Client::CLevel_Boss::CLevel_Boss(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CLevel(pDevice, pContext)
{
}

HRESULT Client::CLevel_Boss::Initialize()
{
	m_pGameInstance.lock()->Play_BGM(L"Boss1Bgm", 0.1f);

    if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
        return E_FAIL;
    if (FAILED(Ready_Layer_Player(L"Layer_Player")))
        return E_FAIL;
    if (FAILED(Ready_Layer_Monster(L"Layer_Monster")))
        return E_FAIL;
    if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
        return E_FAIL;

	m_pBoss->PlayBornSound();

    return S_OK;
}

void Client::CLevel_Boss::Tick(_float fTimeDelta)
{
    if (m_pGameInstance.lock()->GetKeyDown(DIK_F4))
        m_pGameInstance.lock()->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BOSS2));
}

HRESULT Client::CLevel_Boss::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("보스 레벨입니다."));
#endif

    return S_OK;
}

HRESULT Client::CLevel_Boss::Ready_Layer_Camera(const wstring& strLayerTag)
{
	if (m_pPlayableChar == nullptr)
		return E_FAIL;

	// 플레이어 combined메인카메라
	Vector3 vLook = m_pPlayableChar->Get_Transform()->Get_Look();

	shared_ptr<CCamera_Combined> pCam = CCamera_Combined::Create(XMConvertToRadians(60.f), 1.f, 1000.f, (_float)g_iWinSizeX / (_float)g_iWinSizeY, m_pPlayableChar, -vLook * 6.f);
	if (FAILED(m_pGameInstance.lock()->Add_Camera(LEVEL_BOSS, L"Player_Combined", pCam)))
		return E_FAIL;

	m_pPlayableChar->Set_CombinedCamera(pCam);

	// 입장 연출 카메라(보스 컷신)
	{
		CActionCamera::INIT_DESC actionCamDesc;
		actionCamDesc.pRefObject = m_pBoss;
		actionCamDesc.keyFrames.resize(3);

		actionCamDesc.keyFrames[0].vPosition = Vector3(-2.5f,3.1f,-1.7f);
		actionCamDesc.keyFrames[0].vLookPosition = Vector3(0.f,1.7f,0.f);
		actionCamDesc.keyFrames[0].fKeyframeTime = 0.f;
		actionCamDesc.keyFrames[0].bMoveLerp = true;

		actionCamDesc.keyFrames[1].vPosition = Vector3(-2.5f, 3.1f, -0.8f);
		actionCamDesc.keyFrames[1].vLookPosition = Vector3(0.f, 1.7f, 0.f);
		actionCamDesc.keyFrames[1].fKeyframeTime = 1.f;
		actionCamDesc.keyFrames[1].bMoveLerp = true;

		actionCamDesc.keyFrames[2].vPosition = Vector3(-2.5f, 3.1f, 2.35f);
		actionCamDesc.keyFrames[2].vLookPosition = Vector3(0.f, 1.7f, 0.f);
		actionCamDesc.keyFrames[2].fKeyframeTime = 2.5f;
		actionCamDesc.keyFrames[2].bMoveLerp = true;

		shared_ptr<CActionCamera> pActionCam = CActionCamera::Create(m_pDevice, m_pContext, XMConvertToRadians(60.f), 0.1f, 1000.f, (_float)g_iWinSizeX / (_float)g_iWinSizeY, actionCamDesc, LEVEL_BOSS, L"Player_Combined");
		if (FAILED(m_pGameInstance.lock()->Add_Camera(LEVEL_BOSS, L"Boss_Born", pActionCam)))
			return E_FAIL;

		m_pGameInstance.lock()->Set_MainCamera(LEVEL_BOSS, L"Boss_Born");
	}

	// 궁 연출 카메라
	{
		CActionCamera::INIT_DESC actionCamDesc;
		actionCamDesc.pRefObject = m_pPlayableChar;

		actionCamDesc.keyFrames.resize(7);

		actionCamDesc.keyFrames[0].vPosition = Vector3(-0.7f, 1.36f, 1.3f);
		actionCamDesc.keyFrames[0].vLookPosition = Vector3(0.f, 1.4f, 0.f);
		actionCamDesc.keyFrames[0].fKeyframeTime = 0.f;
		actionCamDesc.keyFrames[0].bMoveLerp = true;

		actionCamDesc.keyFrames[1].vPosition = Vector3(-1.3f, 1.46f, 0.3f);
		actionCamDesc.keyFrames[1].vLookPosition = Vector3(0.f, 1.4f, 0.f);
		actionCamDesc.keyFrames[1].fKeyframeTime = 0.3f;
		actionCamDesc.keyFrames[1].bMoveLerp = true;

		actionCamDesc.keyFrames[2].vPosition = Vector3(-0.5f, 1.5f, 0.76f);
		actionCamDesc.keyFrames[2].vLookPosition = Vector3(0.f, 1.4f, 0.f);
		actionCamDesc.keyFrames[2].fKeyframeTime = 0.6f;
		actionCamDesc.keyFrames[2].bMoveLerp = true;

		actionCamDesc.keyFrames[3].vPosition = Vector3(-0.2f, 1.52f, 0.75f);
		actionCamDesc.keyFrames[3].vLookPosition = Vector3(0.f, 1.4f, 0.f);
		actionCamDesc.keyFrames[3].fKeyframeTime = 0.9f;
		actionCamDesc.keyFrames[3].bMoveLerp = true;

		actionCamDesc.keyFrames[4].vPosition = Vector3(0.f, 1.54f, 0.9f);
		actionCamDesc.keyFrames[4].vLookPosition = Vector3(0.f, 1.4f, 0.f);
		actionCamDesc.keyFrames[4].fKeyframeTime = 1.2f;
		actionCamDesc.keyFrames[4].bMoveLerp = true;

		actionCamDesc.keyFrames[5].vPosition = Vector3(0.f, 1.54f, 0.9f);
		actionCamDesc.keyFrames[5].vLookPosition = Vector3(0.f, 1.4f, 0.f);
		actionCamDesc.keyFrames[5].fKeyframeTime = 1.3f;
		actionCamDesc.keyFrames[5].bMoveLerp = true;

		actionCamDesc.keyFrames[6].vPosition = Vector3(0.f, 1.7f, 3.7f);
		actionCamDesc.keyFrames[6].vLookPosition = Vector3(0.f, 1.4f, 0.f);
		actionCamDesc.keyFrames[6].fKeyframeTime = 2.3f;
		actionCamDesc.keyFrames[6].bMoveLerp = true;

		shared_ptr<CActionCamera> pActionCam = CActionCamera::Create(m_pDevice, m_pContext, XMConvertToRadians(60.f), 0.1f, 1000.f, (_float)g_iWinSizeX / (_float)g_iWinSizeY, actionCamDesc, LEVEL_BOSS, L"Player_Combined");
		if (FAILED(m_pGameInstance.lock()->Add_Camera(LEVEL_BOSS, L"Player_Ultimate", pActionCam)))
			return E_FAIL;
	}

	// 퇴장 연출 카메라
	{
		CActionCamera::INIT_DESC actionCamDesc;
		actionCamDesc.pRefObject = m_pBoss;

		// 17초
		actionCamDesc.keyFrames.resize(5);

		actionCamDesc.keyFrames[0].vPosition = Vector3(0.f, 2.5f, 2.f);
		actionCamDesc.keyFrames[0].vLookPosition = Vector3(0.f, 0.f, 0.f);
		actionCamDesc.keyFrames[0].fKeyframeTime = 0.f;
		actionCamDesc.keyFrames[0].bMoveLerp = true;

		actionCamDesc.keyFrames[1].vPosition = Vector3(0.f, 1.8f, 2.f);
		actionCamDesc.keyFrames[1].vLookPosition = Vector3(0.f, 1.8f, 0.f);
		actionCamDesc.keyFrames[1].fKeyframeTime = 1.2f;
		actionCamDesc.keyFrames[1].bMoveLerp = true;

		actionCamDesc.keyFrames[2].vPosition = Vector3(0.f, 1.8f, 2.f);
		actionCamDesc.keyFrames[2].vLookPosition = Vector3(0.f, 1.8f, 0.f);
		actionCamDesc.keyFrames[2].fKeyframeTime = 2.2f;
		actionCamDesc.keyFrames[2].bMoveLerp = true;

		actionCamDesc.keyFrames[3].vPosition = Vector3(0.f, 1.8f, 6.5f);
		actionCamDesc.keyFrames[3].vLookPosition = Vector3(0.f, 1.8f, 0.f);
		actionCamDesc.keyFrames[3].fKeyframeTime = 2.4f;
		actionCamDesc.keyFrames[3].bMoveLerp = true;

		actionCamDesc.keyFrames[4].vPosition = Vector3(0.f, 1.8f, 6.5f);
		actionCamDesc.keyFrames[4].vLookPosition = Vector3(0.f, 1.8f, 0.f);
		actionCamDesc.keyFrames[4].fKeyframeTime = 17.f;
		actionCamDesc.keyFrames[4].bMoveLerp = true;

		shared_ptr<CActionCamera> pActionCam = CActionCamera::Create(m_pDevice, m_pContext, XMConvertToRadians(60.f), 0.1f, 1000.f, (_float)g_iWinSizeX / (_float)g_iWinSizeY, actionCamDesc, LEVEL_BOSS, L"Player_Combined", LEVEL_BOSS2, true);
		if (FAILED(m_pGameInstance.lock()->Add_Camera(LEVEL_BOSS, L"Boss_Die", pActionCam)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT Client::CLevel_Boss::Ready_Layer_BackGround(const wstring& strLayerTag)
{
    CMapObject::INIT_DESC mapDesc;
    mapDesc.eLevel = LEVEL_BOSS;
    mapDesc.strModelKey = L"Scene01906 (merge)";
    mapDesc.vInitPos = Vector3(0.f, 0.f, 0.f);
    shared_ptr<CMapObject> pMap = CMapObject::Create(m_pDevice, m_pContext, mapDesc);
    if (FAILED(m_pGameInstance.lock()->Add_Object(LEVEL_BOSS, strLayerTag, pMap, &g_TimeScale_Player)))
        return E_FAIL;

    shared_ptr<CNavMeshProp> pNavMesh = m_pGameInstance.lock()->Get_NavMesh(LEVEL_BOSS, L"Scene01906");
    if (FAILED(m_pGameInstance.lock()->Set_NavMesh(LEVEL_BOSS, pNavMesh, pMap)))
        return E_FAIL;

    return S_OK;
}

HRESULT Client::CLevel_Boss::Ready_Layer_Player(const wstring& strLayerTag)
{
    CPlayableVera::INIT_DESC playerDesc;
    playerDesc.ePropLevel = LEVEL_STATIC;
    playerDesc.eCreateLevel = LEVEL_BOSS;
    playerDesc.strModelKey = L"R3WeilaMd010021 (merge)";
    playerDesc.vInitPos = Vector3(-135.f, 25.f, -95.f);
    playerDesc.iHp = 1000;
    playerDesc.iAttackPower = 15;
    m_pPlayableChar = CPlayableVera::Create(m_pDevice, m_pContext, playerDesc);
	m_pPlayableChar->Get_Transform()->Rotation(Vector4(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Add_Object(LEVEL_BOSS, strLayerTag, m_pPlayableChar, &g_TimeScale_Player)))
        return E_FAIL;

    return S_OK;
}

HRESULT Client::CLevel_Boss::Ready_Layer_Monster(const wstring& strLayerTag)
{
    CMonsterBase::INIT_DESC monsterDesc;
    monsterDesc.eLevel = LEVEL_BOSS;
    monsterDesc.strModelKey = L"Mb1BruceloongMd010001";
    monsterDesc.vInitPos = Vector3(-135.f, 25.f, -115.f);
    monsterDesc.fSpeed = 2.f;
    monsterDesc.iAttackPower = 10;
    monsterDesc.iHp = 100;
    monsterDesc.iShield = 100;
    monsterDesc.pTarget = m_pPlayableChar;
	m_pBoss = CBoss_Pushin::Create(m_pDevice, m_pContext, monsterDesc);
    if (FAILED(m_pGameInstance.lock()->Add_Object(LEVEL_BOSS, strLayerTag, m_pBoss, &g_TimeScale_Monster)))
        return E_FAIL;

	m_pPlayableChar->Set_Target(m_pBoss);

    return S_OK;
}

shared_ptr<CLevel_Boss> Client::CLevel_Boss::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CLevel_Boss> pInstance = make_shared<CLevel_Boss>(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Boss");
        pInstance.reset();
    }

    return pInstance;
}
