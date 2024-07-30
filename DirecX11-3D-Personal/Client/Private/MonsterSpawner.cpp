#include "pch.h"
#include "MonsterSpawner.h"
#include "MonsterBase.h"
#include "ParticleEffect.h"
#include "TableSawMonster.h"
#include "SmallRobot.h"

Client::CMonsterSpawner::CMonsterSpawner(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice, pContext)
{
}

HRESULT Client::CMonsterSpawner::Initialize(const INIT_DESC& initDesc)
{
	if (FAILED(__super::Initialize(initDesc.vInitPos)))
		return E_FAIL;

	eCreateLevel = initDesc.eCreateLevel;

	// ************************************* 
	// 스폰대상들을 미리 생성해서 집어넣어놓기
	// 동시에 생성 파티클을 집어넣기
	// *************************************
	shared_ptr<CParticleEffect> pParticle = nullptr;
	CParticleEffect::INIT_DESC particleDesc;
	particleDesc.eLevel = initDesc.eCreateLevel;
	particleDesc.FXPrefabPath = L"../Bin/EffectPrefab/ParticleEffect/MonsterSpawn.Particle";

	shared_ptr<CTableSawMonster> pSawMan = nullptr;
	CTableSawMonster::INIT_DESC SawManDesc;
	SawManDesc.eLevel = initDesc.eCreateLevel;
	SawManDesc.strModelKey = L"Yuanding";
	SawManDesc.fSpeed = 2.f;
	SawManDesc.iAttackPower = 10;
	SawManDesc.iHp = 10000;
	SawManDesc.pTarget = initDesc.pTargetChar;

	for (size_t SawManCnt = 0; SawManCnt < initDesc.vSawManPositions.size(); ++SawManCnt)
	{
		SawManDesc.vInitPos = initDesc.vSawManPositions[SawManCnt];

		pSawMan = CTableSawMonster::Create(m_pDevice, m_pContext, SawManDesc);
		if (nullptr == pSawMan)
		{
			MSG_BOX("CMonsterSpawner::Initialize");
			return E_FAIL;
		}
		pSawMan->Set_Activate(false);
		m_MonsterList.push_back(pSawMan);

		particleDesc.vInitPos = initDesc.vSawManPositions[SawManCnt];
		pParticle = CParticleEffect::Create(m_pDevice, m_pContext, particleDesc);
		m_SpawnParticleList.push_back(pParticle);
	}

	shared_ptr<CSmallRobot> pRobot = nullptr;
	CSmallRobot::INIT_DESC RobotDesc;
	RobotDesc.eLevel = initDesc.eCreateLevel;
	RobotDesc.strModelKey = L"Mo1YeyaMd000001";
	RobotDesc.fSpeed = 2.f;
	RobotDesc.iAttackPower = 10;
	RobotDesc.iHp = 100;
	RobotDesc.pTarget = initDesc.pTargetChar;
	RobotDesc.iShield = 100;

	for (size_t RobotCnt = 0; RobotCnt < initDesc.vRobotPositions.size(); ++RobotCnt)
	{
		RobotDesc.vInitPos = initDesc.vRobotPositions[RobotCnt];

		pRobot = CSmallRobot::Create(m_pDevice, m_pContext, RobotDesc);
		if (nullptr == pRobot)
		{
			MSG_BOX("CMonsterSpawner::Initialize");
			return E_FAIL;
		}

		pRobot->Set_Activate(false);
		m_MonsterList.push_back(pRobot);

		particleDesc.vInitPos = initDesc.vRobotPositions[RobotCnt];
		pParticle = CParticleEffect::Create(m_pDevice, m_pContext, particleDesc);
		m_SpawnParticleList.push_back(pParticle);
	}

	if (FAILED(Add_Component()))
	{
		MSG_BOX("CMonsterSpawner::Initialize");
		return E_FAIL;
	}

    return S_OK;
}

_int Client::CMonsterSpawner::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	m_pTrigger->Tick(matWorld);

	return OBJ_ALIVE;
}

void Client::CMonsterSpawner::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_pTrigger->IsActivate())
		m_pGameInstance.lock()->Add_DebugComponent(m_pTrigger);
#endif
}

HRESULT Client::CMonsterSpawner::Add_Component()
{
	CBounding_Sphere::INIT_DESC sphereColDesc;
	sphereColDesc.vCenter = Vector3(0.f, 0.f, 0.f);
	sphereColDesc.fRadius = 5.f;

	CCollider::INIT_DESC ColDesc;
	ColDesc.eType = CCollider::TYPE_SPHERE;
	ColDesc.iCreateLevelIndex = eCreateLevel;
	ColDesc.iLayerType = Col_Environment;
	ColDesc.pInitDesc = make_shared<CBounding_Sphere::INIT_DESC>(sphereColDesc);;
	ColDesc.pOwner = shared_from_this();

	m_pTrigger = CCollider::Create(m_pDevice, m_pContext, ColDesc);
	if (nullptr == m_pTrigger)
		return E_FAIL;

    return S_OK;
}

void Client::CMonsterSpawner::Spawn()
{
	m_pGameInstance.lock()->Play_Sound(L"Spawn", 0.5f);

	for (auto& pMonster : m_MonsterList)
	{
		pMonster->Set_Activate(true);
		m_pGameInstance.lock()->Add_Object(eCreateLevel, L"Layer_Monster", pMonster, &g_TimeScale_Monster);
	}

	for (auto& pParticle : m_SpawnParticleList)
	{
		m_pGameInstance.lock()->Add_Object(eCreateLevel, L"Layer_SpawnEffect", pParticle, &g_TimeScale_Monster);
		pParticle->PlayOnce();
	}
}

void Client::CMonsterSpawner::OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
{
	if (Col_Player == pCollider->Get_ColliderLayer())
	{
		m_pTrigger->Set_Activate(false);
		Spawn();
	}
}

shared_ptr<CMonsterSpawner> Client::CMonsterSpawner::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc)
{
	shared_ptr<CMonsterSpawner> pInstance = make_shared<CMonsterSpawner>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(initDesc)))
	{
		MSG_BOX("Failed to Created : CMonsterSpawner");
		pInstance.reset();
	}

	return pInstance;
}
