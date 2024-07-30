#include"pch.h"
#include "SmallRobot.h"
#include "ParticleEffect.h"

Client::CSmallRobot::CSmallRobot(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMonsterBase(pDevice, pContext)
{
	m_StateFunctions.resize(MonsterStateEnd);
	m_strMovableObjName = L"유압";
}

void Client::CSmallRobot::Set_Activate(_bool bActivate)
{
	m_MapColliderCom[L"Col_Body"]->Set_Activate(bActivate);
}

HRESULT Client::CSmallRobot::Initialize(const INIT_DESC& monsterDesc)
{
	if (FAILED(__super::Initialize(monsterDesc)))
		return E_FAIL;

	if (FAILED(Add_StateFunction()))
		return E_FAIL;

	if (FAILED(Add_Component(monsterDesc.eLevel, monsterDesc.strModelKey))) return E_FAIL;

	// 피격 이펙트 생성
	CParticleEffect::INIT_DESC particleDesc;
	particleDesc.eLevel = monsterDesc.eLevel;
	particleDesc.FXPrefabPath = L"../Bin/EffectPrefab/ParticleEffect/VeraHit.Particle";
	m_pHitParticle = CParticleEffect::Create(m_pDevice, m_pContext, particleDesc);
	if (FAILED(m_pGameInstance.lock()->Add_Object(monsterDesc.eLevel, L"Layer_MonsterEffect", m_pHitParticle, &g_TimeScale_Player)))
	{
		MSG_BOX("CSmallRobot::Initialize");
		return E_FAIL;
	}

	Change_State(Idle);
	Change_Anim(18);

	return S_OK;
}

void Client::CSmallRobot::Priority_Tick(_float fTimeDelta)
{
	if (m_bDead)
		return;

	__super::Priority_Tick(fTimeDelta);
}

_int Client::CSmallRobot::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_ALIVE;

	m_pMeshCom->Play_Animation(fTimeDelta, m_vRootMotionVelocity, m_bLastAnimFrameCalled, m_dCurrentAnimFrame, true, true, true);

	if (m_iCurrentState < Walk)
	{
		if (m_StopWatchs[SW_3].Start == false)
			m_StopWatchs[SW_3].StartCheck();

		if (m_StopWatchs[SW_3].fTime > 6.f)
			m_bCanAttack = true;
	}

	if ((m_iCurrentState >= Attack1) &&
		(m_iCurrentState <= Attack4))
	{
		auto Collider = m_MapColliderCom[L"Col_Attack"];

		if (m_dCurrentAnimFrame >= 0.55 && m_dCurrentAnimFrame <= 0.75 && !Collider->IsActivate())
			Collider->Set_Activate(true);
		else if ((m_dCurrentAnimFrame < 0.55|| m_dCurrentAnimFrame > 0.75) && Collider->IsActivate())
			Collider->Set_Activate(false);
	}

	__super::Tick(fTimeDelta);

	Move();

	if (m_iCurrentState == Walk)
		m_pTransformCom->Go_Right(moveVelocity.Length() * fTimeDelta, m_pNavigationCom);
	else if (m_iCurrentState == RunL)
		m_pTransformCom->Go_Right(-moveVelocity.Length() * fTimeDelta, m_pNavigationCom);
	else if (m_iCurrentState == Run || m_iCurrentState == Attack4)
		m_pTransformCom->Go_Straight(moveVelocity.Length() * fTimeDelta, m_pNavigationCom);

	//if (m_vRootMotionVelocity.Length() > 0.f)
	//{
	//	m_pTransformCom->Go_Right(m_vRootMotionVelocity.x, m_pNavigationCom);
	//	m_pTransformCom->Go_Straight(-m_vRootMotionVelocity.z, m_pNavigationCom);
	//}

	return OBJ_ALIVE;
}

void Client::CSmallRobot::Late_Tick(_float fTimeDelta)
{
	if (m_bDead)
		return;

	__super::Late_Tick(fTimeDelta);

	m_pTransformCom->Go_Straight(moveVelocity.Length() * fTimeDelta);

	if (FAILED(m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;
}

HRESULT Client::CSmallRobot::Render()
{
	if (FAILED(__super::Render()))
			return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_pMeshCom->Get_MeshCount(); ++i)
	{
		// 텍스처 바인딩
		if (FAILED(m_pMeshCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pMeshCom->Bind_BoneMatrices("g_BoneMatrices", i)))
			return E_FAIL;

		m_pMeshCom->Render(i, 0);
	}

	return S_OK;
}

HRESULT Client::CSmallRobot::Add_StateFunction()
{
#pragma region 스탠딩
	m_StateFunctions[MonsterState::Idle].OnEnterState = [&](_float fTimeDelta) { Change_Anim(18); };
#pragma endregion

#pragma region 이동관련
	// 걷기
	m_StateFunctions[MonsterState::Walk].OnEnterState = [&](_float fTimeDelta) { Change_Anim(22); };
	m_StateFunctions[MonsterState::Walk].OnState = [&](_float fTimeDelta) { if (moveVelocity.Length() < 0.1f) { Change_State(Idle); } };
	// 뒤로걷기
	m_StateFunctions[MonsterState::RunB].OnEnterState = [&](_float fTimeDelta) { Change_Anim(15); };
	m_StateFunctions[MonsterState::RunB].OnState = [&](_float fTimeDelta) { if (moveVelocity.Length() < 0.1f) { Change_State(Idle); } };
	// 오른쪽걷기
	m_StateFunctions[MonsterState::RunR].OnEnterState = [&](_float fTimeDelta) { Change_Anim(17); };
	m_StateFunctions[MonsterState::RunR].OnState = [&](_float fTimeDelta) { if (moveVelocity.Length() < 0.1f) { Change_State(Idle); } };
	// 왼쪽걷기
	m_StateFunctions[MonsterState::RunL].OnEnterState = [&](_float fTimeDelta) { Change_Anim(16); };
	m_StateFunctions[MonsterState::RunL].OnState = [&](_float fTimeDelta) { if (moveVelocity.Length() < 0.1f) { Change_State(Idle); } };
	// 달리기
	m_StateFunctions[MonsterState::Run].OnEnterState = [&](_float fTimeDelta) { Change_Anim(14); };
	m_StateFunctions[MonsterState::Run].OnState = [&](_float fTimeDelta) { if (moveVelocity.Length() < 0.1f) { Change_State(Idle); } };
#pragma endregion

#pragma region 공격관련
	// 공격1 : 근거리 찌르기 
	m_StateFunctions[MonsterState::Attack1].OnEnterState = [&](_float fTimeDelta) { Change_Anim(0); };
	m_StateFunctions[MonsterState::Attack1].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) { Change_State(Idle); } };
	m_StateFunctions[MonsterState::Attack1].OnExitState = [&](_float fTimeDelta) { m_bCanAttack = false; };
	// 공격2 : 좀느리게 발차기
	m_StateFunctions[MonsterState::Attack2].OnEnterState = [&](_float fTimeDelta) { Change_Anim(1); };
	m_StateFunctions[MonsterState::Attack2].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) { Change_State(Idle); } };
	m_StateFunctions[MonsterState::Attack2].OnExitState = [&](_float fTimeDelta) { m_bCanAttack = false; };
	// 공격3 : 빠르게 발차기
	m_StateFunctions[MonsterState::Attack3].OnEnterState = [&](_float fTimeDelta) { Change_Anim(2); };
	m_StateFunctions[MonsterState::Attack3].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) { Change_State(Idle); } };
	m_StateFunctions[MonsterState::Attack3].OnExitState = [&](_float fTimeDelta) { m_bCanAttack = false; };
	// 공격4 : 점프공격
	m_StateFunctions[MonsterState::Attack4].OnEnterState = [&](_float fTimeDelta) { Change_Anim(3); m_fJumpDist = m_fTargetDist; };
	m_StateFunctions[MonsterState::Attack4].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) { Change_State(Idle); } };
	m_StateFunctions[MonsterState::Attack4].OnExitState = [&](_float fTimeDelta) { m_bCanAttack = false; };
#pragma endregion

#pragma region 피격관련
	// BeHitFly
 	m_StateFunctions[MonsterState::BeHitFly].OnEnterState = [&](_float fTimeDelta) { Change_Anim(4); };
	m_StateFunctions[MonsterState::BeHitFly].OnState = [&](_float fTimeDelta) { if (moveVelocity.Length() < 0.1f) { Change_State(Idle); } };
	// falldown
 	m_StateFunctions[MonsterState::FallDown].OnEnterState = [&](_float fTimeDelta) { Change_Anim(6); };
	m_StateFunctions[MonsterState::FallDown].OnState = [&](_float fTimeDelta) { if (moveVelocity.Length() < 0.1f) { Change_State(Idle); } };
	// Hit1, 2, 3, 4
 	m_StateFunctions[MonsterState::hit].OnEnterState = [&](_float fTimeDelta) {
		if (m_bCanAttack == false)
			m_bCanAttack = true;

		switch (m_iHitStack)
		{
		case 0:
			Change_Anim(7);
			break;
		case 1:
			Change_Anim(8);
			break;
		case 2:
			Change_Anim(9);
			break;
		case 3:
			Change_Anim(10);
			break;
		}
		};
	m_StateFunctions[MonsterState::hit].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) { Change_State(Idle); } };
	m_StateFunctions[MonsterState::hit].OnExitState = [&](_float fTimeDelta) {
		m_iHitStack = (m_iHitStack + 1) % 4;
		if (m_iNextState != m_iHitStack) m_iHitStack = 0;};
	// HitDown
 	m_StateFunctions[MonsterState::HitDown].OnEnterState = [&](_float fTimeDelta) { Change_Anim(11); };
	m_StateFunctions[MonsterState::HitDown].OnState = [&](_float fTimeDelta) { if (moveVelocity.Length() < 0.1f) { Change_State(Idle); } };
	// HoverHit
 	m_StateFunctions[MonsterState::OverHit].OnEnterState = [&](_float fTimeDelta) { Change_Anim(12); };
	m_StateFunctions[MonsterState::OverHit].OnState = [&](_float fTimeDelta) { if (moveVelocity.Length() < 0.1f) { Change_State(Idle); } };
	// LieOnFloor
 	m_StateFunctions[MonsterState::LieOnFloor].OnEnterState = [&](_float fTimeDelta) { Change_Anim(13); };
	m_StateFunctions[MonsterState::LieOnFloor].OnState = [&](_float fTimeDelta) { if (moveVelocity.Length() < 0.1f) { Change_State(Idle); } };
	// StandUp
 	m_StateFunctions[MonsterState::StandUp].OnEnterState = [&](_float fTimeDelta) { Change_Anim(19); };
	m_StateFunctions[MonsterState::StandUp].OnState = [&](_float fTimeDelta) { if (moveVelocity.Length() < 0.1f) { Change_State(Idle); } };
	// Death
	m_StateFunctions[MonsterState::Death].OnEnterState = [&](_float fTimeDelta) { Change_Anim(11); };
	m_StateFunctions[MonsterState::Death].OnState = [&](_float fTimeDelta) {
		if (m_bLastAnimFrameCalled)
		{
			m_bDead = true;
			m_MapColliderCom[L"Col_Body"]->Set_Activate(false);
		}
		};
#pragma endregion

	return S_OK;
}

HRESULT Client::CSmallRobot::Add_Component(LEVEL eLevel, const wstring& strModelKey)
{
	if (FAILED(__super::Add_Component(eLevel, eLevel, strModelKey)))
		return E_FAIL;

	// 충돌 콜라이더
	CBounding_Sphere::INIT_DESC sphereColDesc;
	sphereColDesc.vCenter = Vector3(0.f, 0.7f, 0.f);
	sphereColDesc.fRadius = 0.7f;

	CCollider::INIT_DESC colliderDesc;
	colliderDesc.eType = CCollider::TYPE_SPHERE;
	colliderDesc.iLayerType = Col_Monster;
	colliderDesc.pInitDesc = make_shared<CBounding_Sphere::INIT_DESC>(sphereColDesc);
	colliderDesc.pOwner = shared_from_this();
	colliderDesc.iCreateLevelIndex = eLevel;
	m_MapColliderCom.emplace(L"Col_Body", CCollider::Create(m_pDevice, m_pContext, colliderDesc));

	// 공격 콜라이더
	sphereColDesc.vCenter = Vector3(0.f, 0.7f, 0.f);
	sphereColDesc.fRadius = 1.1f;

	colliderDesc.eType = CCollider::TYPE_SPHERE;
	colliderDesc.iLayerType = Col_MonsterAttack;
	colliderDesc.pInitDesc = make_shared<CBounding_Sphere::INIT_DESC>(sphereColDesc);
	colliderDesc.pOwner = shared_from_this();
	colliderDesc.iCreateLevelIndex = eLevel;
	shared_ptr<CCollider> pAttackCol = CCollider::Create(m_pDevice, m_pContext, colliderDesc);
	pAttackCol->Set_Activate(false);

	m_MapColliderCom.emplace(L"Col_Attack", pAttackCol);

	return S_OK;
}

HRESULT Client::CSmallRobot::Bind_ShaderResources()
{
	Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
	Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

	if (FAILED(m_pMeshCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pMeshCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pMeshCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

void Client::CSmallRobot::Move()
{
	// 타겟 주시
	Vector3 LookAtDir = Vector3::Lerp(m_vMyLook, m_vTargetDir, 0.3f * g_TimeScale_Monster);
	LookAtDir.y = 0.f;
	if (m_iCurrentState <= Walk ||
		(m_iCurrentState <= Attack4 && m_iCurrentState >= Attack1 && m_dCurrentAnimFrame < 0.1))
		m_pTransformCom->LookAt(XMVectorSetW(m_vMyPos + LookAtDir, 1.f));

	if (m_iCurrentState <= Walk)
	{
		// 공격에 들어가야한다면
		if (m_bCanAttack)
		{
			// 돌진기 범위까지 달려가기
			if (m_iAttackPatternPhase == 3)
			{
				if (m_fTargetDist > 6.5f)
				{
					moveVelocity = m_vTargetDir * m_fSpeed;

					if (m_iCurrentState != Run)
						Change_State(Run);
				}
				else
				{
					m_bCanAttack = false;
					Change_State(AttackPattern[m_iAttackPatternPhase]);
					m_iAttackPatternPhase++;
					if (m_iAttackPatternPhase > 4)
						m_iAttackPatternPhase = 0;
					m_StopWatchs[SW_3].Clear();
				}
			}
			// 근거리 공격 범위까지 달려가기
			else
			{
				if (m_fTargetDist > 3.5f)
				{
					moveVelocity = m_vTargetDir * m_fSpeed;

					if (m_iCurrentState != Run)
						Change_State(Run);
				}
				else
				{
					m_bCanAttack = false;
					Change_State(AttackPattern[m_iAttackPatternPhase]);
					m_iAttackPatternPhase++;
					if (m_iAttackPatternPhase > 4)
						m_iAttackPatternPhase = 0;
					m_StopWatchs[SW_3].Clear();

					moveVelocity = Vector3(0.f, 0.f, 0.f);
				}
			}
		}
		// 배회상태
		else
		{
			if (m_fTargetDist > 8.f)
			{
				moveVelocity = m_vTargetDir * m_fSpeed;

				if (m_iCurrentState != Run)
					Change_State(Run);
			}
			else
			{
				_int checkmove = static_cast<_int>(m_StopWatchs[SW_3].fTime) / 3;
				if (checkmove % 2 == 0)
				{
					moveVelocity = m_vMyRight * m_fSpeed * 0.25f;

					if (m_iCurrentState != RunR)
						Change_State(RunR);
				}
				else
				{
					moveVelocity = -m_vMyRight * m_fSpeed * 0.25f;

					if (m_iCurrentState != RunL)
						Change_State(RunL);
				}
			}
		}
	}
	else if (m_iCurrentState == Attack4 && m_dCurrentAnimFrame > 0.2 && m_dCurrentAnimFrame < 0.3)
		moveVelocity = Vector3(0.f, 0.f, m_fJumpDist * 0.5f);
}

void Client::CSmallRobot::Hit(_uint iDamage)
{
	if (m_iCurrentState == MonsterState::hit && m_dCurrentAnimFrame < 0.3)
		return;

	__super::Hit(iDamage);

	m_MapColliderCom[L"Col_Attack"]->Set_Activate(false);

	m_iCurrentHp -= iDamage;
	m_iCurrentHp = max(0, m_iCurrentHp);

	if (m_iCurrentHp <= 0 && m_iCurrentState != Death)
	{
		Change_State(Death);
		return;
	}

	Change_State(MonsterState::hit);
}

void Client::CSmallRobot::HardHit(_uint iDamage)
{

}

void Client::CSmallRobot::OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
{
	if (m_iCurrentState == Death)
		return;

	if (pCollider->Get_ColliderLayer() == Col_Player)
	{
		if (pSrcCol == m_MapColliderCom[L"Col_Attack"])
		{
			static_pointer_cast<CMovableObject>(pCollider->Get_Owner().lock())->Hit(m_iAttackPower);
		}
	}
}

void Client::CSmallRobot::OnCollisionStay(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
{
	__super::OnCollisionStay(pSrcCol, pCollider);
}

shared_ptr<CSmallRobot> Client::CSmallRobot::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& monsterDesc)
{
	shared_ptr<CSmallRobot> pInstance = make_shared<CSmallRobot>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(monsterDesc)))
	{
		MSG_BOX("Failed to Created : CSmallRobot");
		pInstance.reset();
	}

	return pInstance;
}
