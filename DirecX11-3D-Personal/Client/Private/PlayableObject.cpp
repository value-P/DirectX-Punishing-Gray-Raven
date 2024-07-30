#include "pch.h"
#include "PlayableObject.h"
#include "Weapon.h"
#include "Layer.h"

Client::CPlayableObject::CPlayableObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CMovableObject(pDevice, pContext)
{
	m_StateFunctions.resize(PlayableStateEnd);
}

HRESULT Client::CPlayableObject::Initialize(const INIT_DESC& initDesc)
{
	if (FAILED(__super::Initialize(initDesc.vInitPos)))
		return E_FAIL;

	if (FAILED(Add_StateFunction()))
		return E_FAIL;

	if (FAILED(Add_Component(initDesc))) return E_FAIL;

	if (FAILED(Add_Children(initDesc.ePropLevel))) return E_FAIL;

	Change_State(Idle);

	return S_OK;
}

void Client::CPlayableObject::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	ComputeTarget();
}

_int Client::CPlayableObject::Tick(_float fTimeDelta)
{
	m_pMeshCom->Play_Animation(fTimeDelta, m_vRootMotionVelocity, m_bLastAnimFrameCalled, m_dCurrentAnimFrame,true, true, true);

	if (m_iCurrentState >= RunStart && m_iCurrentState <= RunL)
		m_vRootMotionVelocity = Vector3(0.f, 0.f, 0.f);

	SkillBallQueueCheck();

	__super::Tick(fTimeDelta);

	if((m_iCurrentState >= MeleeAttack) &&
		(m_iCurrentState <= SkillRedEX))
	{
		auto Collider = m_MapColliderCom[L"Col_Attack"];

		if (m_dCurrentAnimFrame >= 0.1 && m_dCurrentAnimFrame <= 0.2 && !Collider->IsActivate())
			Collider->Set_Activate(true);
		else if ((m_dCurrentAnimFrame < 0.1 || m_dCurrentAnimFrame > 0.2) && Collider->IsActivate())
			Collider->Set_Activate(false);
	}

	Move();

	// 최종moveVelocity 속도 * look방향 transform이동시킴
	if (m_iCurrentState == SpecialMelee)
	{
		m_pTransformCom->Go_Straight(moveVelocity.z * fTimeDelta, m_pNavigationCom);
		m_pTransformCom->Go_Right(moveVelocity.x * fTimeDelta, m_pNavigationCom);
	}
	else
		m_pTransformCom->Go_Straight(moveVelocity.Length() * fTimeDelta, m_pNavigationCom);

	if (m_vRootMotionVelocity.Length() > 0.f)
	{
		m_pTransformCom->Go_Right(m_vRootMotionVelocity.x, m_pNavigationCom);
		m_pTransformCom->Go_Straight(-m_vRootMotionVelocity.z, m_pNavigationCom);
	}

	_float y = m_pNavigationCom->Get_HeightOnNav(m_pTransformCom->Get_Pos());
	m_pTransformCom->Set_Y(y);

	return OBJ_ALIVE;
}

void Client::CPlayableObject::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (FAILED(m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;
}

HRESULT Client::CPlayableObject::Render()
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

void Client::CPlayableObject::KeyInput()
{
	if (m_iCurrentState == SkillUltimate)
		return;

	InputMovement();
	InputDodge();
	InputAttack();

	if (m_pGameInstance.lock()->GetKeyDown(DIK_Q) && !m_bUltimateMode)
		m_SkillInputQueue.push(SkillType::BLUE);
	if (m_pGameInstance.lock()->GetKeyDown(DIK_E) && m_bUltimateMode)
		m_SkillInputQueue.push(SkillType::RED);
	if (m_pGameInstance.lock()->GetKeyDown(DIK_R) && m_bUltimateMode)
		m_SkillInputQueue.push(SkillType::YELLOW);
	if (m_pGameInstance.lock()->GetKeyDown(DIK_T))
		m_SkillInputQueue.push(SkillType::ULTIMATE);

	moveVelocity.Normalize();
	moveVelocity *= m_fSpeed;
}

void Client::CPlayableObject::SkillBallQueueCheck()
{
	if (false == m_SkillInputQueue.empty())
	{
		if ((m_iCurrentState < SkillBlue || m_iCurrentState > SkillUltimate) ||
			(m_iCurrentState >= SkillBlue && m_iCurrentState <= SkillUltimate && m_dCurrentAnimFrame > 0.3))
		{
			switch (m_SkillInputQueue.front())
			{
			case SkillType::BLUE:
				m_SkillInputQueue.pop();
				Change_State(SkillBlue);
				break;
			case SkillType::RED:
				m_SkillInputQueue.pop();
				Change_State(SkillRed);
				break;
			case SkillType::YELLOW:
				m_SkillInputQueue.pop();
				Change_State(SkillYellow);
				break;
			case SkillType::ULTIMATE:
				m_SkillInputQueue.pop();
				Change_State(SkillUltimate);
				break;
			}
		}
	}
}

void Client::CPlayableObject::InputDodge()
{
	if (m_pGameInstance.lock()->GetMouseButtonDown(DIM_RB))
	{
		if (moveVelocity.Length() > 0.1f && m_dCurrentAnimFrame > 0.4)
			Change_State(Dodge);
		else if (m_StopWatchs[SW_0].Start == false)
			m_StopWatchs[SW_0].StartCheck();


	}

	if (m_pGameInstance.lock()->GetMouseButton(DIM_RB))
	{
		if (m_StopWatchs[SW_0].fTime > 0.4f)
		{
			if (m_iCurrentState < CorePassiveStart)
			{
				m_StopWatchs[SW_0].Clear();
				Change_State(CorePassiveStart);
			}			
		}
	}

	if (m_pGameInstance.lock()->GetMouseButtonUp(DIM_RB))
	{
		m_StopWatchs[SW_0].Clear();

		switch (m_iCurrentState)
		{
		case CorePassiveAttack:
			if (m_dCurrentAnimFrame > 0.4)
				Change_State(Dodge);
			break;
		case CorePassiveStart:
		case CorePassiveGauging:
		case CorePassiveParring:
			Change_State(CorePassiveAttack);
			break;
		case Dodge:
			if (m_dCurrentAnimFrame > 0.4)
				Change_State(Dodge);
			break;
		default:
			Change_State(Dodge);
			break;
		}
	}
}

void Client::CPlayableObject::InputAttack()
{
	if (m_pGameInstance.lock()->GetKeyDown(DIK_SPACE))
	{
		if (m_iCurrentState >= Idle && m_iCurrentState <= Stop)
			Change_State(MeleeAttack);
		else
		{
			switch (m_iCurrentState)
			{
			case MeleeAttack:
				if ((m_iMeleeStack < 4 && m_dCurrentAnimFrame > 0.1) ||
					(m_iMeleeStack == 4 && m_dCurrentAnimFrame > 0.4))
					Change_State(MeleeAttack);
				break;
			case SkillRed:
				if (m_dCurrentAnimFrame > 0.3)
					Change_State(SkillRedEX);
				break;
			case SkillYellow:
				if (m_dCurrentAnimFrame > 0.4)
					Change_State(SkillYellowEX);
				break;
			case Dodge:
				if (m_dCurrentAnimFrame > 0.4)
					Change_State(SpecialMelee);
				break;
			case SpecialMelee:
				if (m_dCurrentAnimFrame > 0.7)
					Change_State(SpecialMelee);
				break;
			}
		}
		
	}
}

void Client::CPlayableObject::InputMovement()
{
	if (m_pGameInstance.lock()->GetKey(DIK_W))
		moveVelocity.z = 1.f;
	else if (m_pGameInstance.lock()->GetKey(DIK_S))
		moveVelocity.z = -1.f;

	if (m_pGameInstance.lock()->GetKey(DIK_D))
		moveVelocity.x = 1.f;
	else if (m_pGameInstance.lock()->GetKey(DIK_A))
		moveVelocity.x = -1.f;

	if (m_pGameInstance.lock()->GetKeyUp(DIK_W) || m_pGameInstance.lock()->GetKeyUp(DIK_S))
		moveVelocity.z = 0.f;
	if (m_pGameInstance.lock()->GetKeyUp(DIK_D) || m_pGameInstance.lock()->GetKeyUp(DIK_A))
		moveVelocity.x = 0.f;
}

HRESULT Client::CPlayableObject::Add_StateFunction()
{	
#pragma region 이동관련

	// 달리기 직후
	m_StateFunctions[PlayableState::RunStart].OnEnterState = [&](_float fTimeDelta) {
		if (m_bUltimateMode)
		{
			m_pWeapon->Change_Anim(62);
			Change_Anim(76);
		}
		else
		{
			m_pWeapon->Change_Anim(65);
			Change_Anim(77);
		}
		};
	m_StateFunctions[PlayableState::RunStart].OnState = [&](_float fTimeDelta) {
		if (m_bLastAnimFrameCalled) { Change_State(Run); }
		else if (moveVelocity.Length() < 0.1f) { Change_State(RunStartEnd); }
		};

	// 달리기 직후 이전에 멈출때
	m_StateFunctions[PlayableState::RunStartEnd].OnEnterState = [&](_float fTimeDelta) { 
		if (m_bUltimateMode)
		{
			m_pWeapon->Change_Anim(63);
			Change_Anim(78);
		}
		else
		{
			m_pWeapon->Change_Anim(65);
			Change_Anim(79);
		}
		};
	m_StateFunctions[PlayableState::RunStartEnd].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) { Change_State(Idle2); }  };

	// 달리다가 멈추기 상태
	m_StateFunctions[PlayableState::Stop].OnEnterState = [&](_float fTimeDelta) {
		if (m_bUltimateMode) {
			m_pWeapon->Change_Anim(68);
			Change_Anim(89);
		}
		else 
		{
			m_pWeapon->Change_Anim(65);
			Change_Anim(88);
		} 
		};
	m_StateFunctions[PlayableState::Stop].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) {
 		Change_State(Idle2); }  };

	// 정지모션 2상태
	m_StateFunctions[PlayableState::Idle2].OnEnterState = [&](_float fTimeDelta) {
		m_StopWatchs[SW_Default].StartCheck();  
		if (m_bUltimateMode) {
			m_pWeapon->Change_Anim(66);
			Change_Anim(82);
		} 
		else {
			m_pWeapon->Change_Anim(65);
			Change_Anim(83);
		} 
		};
	m_StateFunctions[PlayableState::Idle2].OnState = [&](_float fTimeDelta) { if (!m_bUltimateMode && m_StopWatchs[SW_Default].fTime >= 4.f) { Change_State(IdleChange); } };
	m_StateFunctions[PlayableState::Idle2].OnExitState = [&](_float fTimeDelta) { m_StopWatchs[SW_Default].Clear(); };
	
	// 정지모션 변경상태(소태도만 있음)
	m_StateFunctions[PlayableState::IdleChange].OnEnterState = [&](_float fTimeDelta) {  Change_Anim(85); };
	m_StateFunctions[PlayableState::IdleChange].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) { Change_State(Idle); } };

	// 정지모션 1 상태(소태도만 있음)
	m_StateFunctions[PlayableState::Idle].OnEnterState = [&](_float fTimeDelta) { m_pWeapon->Change_Anim(64); Change_Anim(81); };

	// 달리기 상태
	m_StateFunctions[PlayableState::Run].OnEnterState = [&](_float fTimeDelta){ 
		if (m_bUltimateMode) {
			m_pWeapon->Change_Anim(59);
			Change_Anim(70);
		}
		else {
			m_pWeapon->Change_Anim(65);
			Change_Anim(71);
		} 
		};
	m_StateFunctions[PlayableState::Run].OnState = [&](_float fTimeDelta){ if (moveVelocity.Length() < 0.1f) { 
		Change_State(Stop); } };

	// 오른쪽달리기
	m_StateFunctions[PlayableState::RunR].OnEnterState = [&](_float fTimeDelta){
		if (m_bUltimateMode) {
			m_pWeapon->Change_Anim(61);
			Change_Anim(75);
		} 
		else {
			m_pWeapon->Change_Anim(65);
			Change_Anim(74);
		} 
		};

	// 왼쪽달리기
	m_StateFunctions[PlayableState::RunL].OnEnterState = [&](_float fTimeDelta){
		if (m_bUltimateMode) {
			m_pWeapon->Change_Anim(60);
			Change_Anim(72);
		}
		else {
			m_pWeapon->Change_Anim(65);
			Change_Anim(73);
		}
		};

	// 회피
	m_StateFunctions[PlayableState::Dodge].OnEnterState = [&](_float fTimeDelta) {
		_bool DodgeForward = false;
		
		if (moveVelocity.Length() > 0.1f)
			DodgeForward = true;
		
		if (m_bUltimateMode)
		{
			if (DodgeForward) {
				Change_Anim(67);
				m_pWeapon->Change_Anim(57);
			} 
			else {
				Change_Anim(69);
				m_pWeapon->Change_Anim(58);
			} 
		}
		else
		{
			m_pWeapon->Change_Anim(65);
			if (DodgeForward) {
				Change_Anim(66);
			}
			else {
				Change_Anim(68);
			} 
		}
		};
	m_StateFunctions[PlayableState::Dodge].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) {
		Change_State(Idle2);
	} };


#pragma endregion

#pragma region 코어 패시브

	// 코어 패시브 시작
	m_StateFunctions[PlayableState::CorePassiveStart].OnEnterState = [&](_float fTimeDelta) {
		if (m_bUltimateMode) {
			m_pWeapon->Change_Anim(23);
			Change_Anim(22);
		}
		else {
			m_pWeapon->Change_Anim(17);
			Change_Anim(15);
		} 
		};
	m_StateFunctions[PlayableState::CorePassiveStart].OnState = [&](_float fTimeDelta) {
		if (m_bLastAnimFrameCalled) { Change_State(CorePassiveGauging); }
		};

	// 코어 패시브 차징
	m_StateFunctions[PlayableState::CorePassiveGauging].OnEnterState = [&](_float fTimeDelta) {
		m_StopWatchs[SW_Default].StartCheck();
		if (m_bUltimateMode) {
			m_pWeapon->Change_Anim(24);
			Change_Anim(23);
		} 
		else {
			m_pWeapon->Change_Anim(18);
			Change_Anim(16);
		} 
		};
	m_StateFunctions[PlayableState::CorePassiveGauging].OnState = [&](_float fTimeDelta) {
		if (m_bLastAnimFrameCalled || m_StopWatchs[SW_Default].fTime >= 4.f)
			Change_State(CorePassiveAttack);
		};
	m_StateFunctions[PlayableState::CorePassiveGauging].OnExitState = [&](_float fTimeDelta) { m_StopWatchs[SW_Default].Clear(); };

	// 코어 패시브 패링
	m_StateFunctions[PlayableState::CorePassiveParring].OnEnterState = [&](_float fTimeDelta) {
		if (m_iParringStack == 0 || m_iParringStack == 2) { m_pWeapon->Change_Anim(37); Change_Anim(36); }
		else if (m_iParringStack == 1) { m_pWeapon->Change_Anim(38); Change_Anim(37); }
		};
	m_StateFunctions[PlayableState::CorePassiveParring].OnState = [&](_float fTimeDelta) {
		if (m_bLastAnimFrameCalled) { Change_State(CorePassiveGauging); }
		};
	m_StateFunctions[CorePassiveParring].OnExitState = [&](_float fTimeDelta) { m_iParringStack++; };

	// 코어 패시브 공격
	m_StateFunctions[PlayableState::CorePassiveAttack].OnEnterState = [&](_float fTimeDelta) {
		if (m_bUltimateMode) {
			m_pWeapon->Change_Anim(25);
			Change_Anim(24);
		}
		else {
			m_pWeapon->Change_Anim(19);
			Change_Anim(17);
		}
		};
	m_StateFunctions[PlayableState::CorePassiveAttack].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) { Change_State(Idle2); } };
	m_StateFunctions[PlayableState::CorePassiveAttack].OnExitState = [&](_float fTimeDelta) { m_iParringStack = 0; };


#pragma endregion

#pragma region 스킬
	// 궁극기
	m_StateFunctions[PlayableState::SkillUltimate].OnEnterState = [&](_float fTimeDelta) {
		if (m_bUltimateMode) {
			m_pWeapon->Change_Anim(22);
			Change_Anim(21);
		}
		else {
			m_pWeapon->Change_Anim(21);
			Change_Anim(20);
		} 
		};
	m_StateFunctions[PlayableState::SkillUltimate].OnState = [&](_float fTimeDelta) {
		if (m_bLastAnimFrameCalled) { Change_State(Idle2); }
		};
	m_StateFunctions[PlayableState::SkillUltimate].OnExitState = [&](_float fTimeDelta) {
		m_bUltimateMode = !m_bUltimateMode;
		};

	// 파랑 볼
	m_StateFunctions[PlayableState::SkillBlue].OnEnterState = [&](_float fTimeDelta) {
		if (m_iPrevState == SkillBlue && m_bBlueSkillChained)
		{
			m_pWeapon->Change_Anim(14);
			Change_Anim(12);
			m_bBlueSkillChained = false;
		}
		else
		{
			m_pWeapon->Change_Anim(13);
			Change_Anim(11); 
			m_bBlueSkillChained = true;
		}
		};
	m_StateFunctions[PlayableState::SkillBlue].OnState = [&](_float fTimeDelta) {
		if(m_bLastAnimFrameCalled) { Change_State(Idle2); }
		};

	// 빨강 볼 (3체인 이후 일반공격시 발동하는 추가타)
	m_StateFunctions[PlayableState::SkillRed].OnEnterState = [&](_float fTimeDelta) {
		m_pWeapon->Change_Anim(11); Change_Anim(10); };
	m_StateFunctions[PlayableState::SkillRed].OnState = [&](_float fTimeDelta) {
		if (m_bLastAnimFrameCalled) { Change_State(Idle2); }
		};

	// 빩랑 볼Ex -> 이후 평타시 특수검기평타로
	m_StateFunctions[PlayableState::SkillRedEX].OnEnterState = [&](_float fTimeDelta) {
		m_pWeapon->Change_Anim(9); Change_Anim(8); };
	m_StateFunctions[PlayableState::SkillRedEX].OnState = [&](_float fTimeDelta) {
		if (m_bLastAnimFrameCalled) { Change_State(Idle2); }
		};

	// 노랑 볼
	m_StateFunctions[PlayableState::SkillYellow].OnEnterState = [&](_float fTimeDelta) {
		m_pWeapon->Change_Anim(16); Change_Anim(14); };
	m_StateFunctions[PlayableState::SkillYellow].OnState = [&](_float fTimeDelta) {
		if (m_bLastAnimFrameCalled) { Change_State(Idle2); }
		};

	// 노랑 볼Ex (3체인 이후 일반공격시 발동하는 추가타) -> 이후 평타시 특수검기평타로
	m_StateFunctions[PlayableState::SkillYellowEX].OnEnterState = [&](_float fTimeDelta) {
		m_pWeapon->Change_Anim(15); Change_Anim(13); };
	m_StateFunctions[PlayableState::SkillYellowEX].OnState = [&](_float fTimeDelta) {
		if (m_bLastAnimFrameCalled) { Change_State(Idle2); }
		};


#pragma endregion

#pragma region 평타

		// 일반공격
	m_StateFunctions[PlayableState::MeleeAttack].OnEnterState = [&](_float fTimeDelta) {
		if (m_bUltimateMode)
		{
			switch (m_iMeleeStack)
			{
			case 0:
				m_pWeapon->Change_Anim(29);
				Change_Anim(28);
				break;
			case 1:
				m_pWeapon->Change_Anim(30);
				Change_Anim(29);
				break;
			case 2:
				m_pWeapon->Change_Anim(31);
				Change_Anim(30);
				break;
			case 3:
				m_pWeapon->Change_Anim(32);
				Change_Anim(31);
				break;
			case 4:
				m_pWeapon->Change_Anim(33);
				Change_Anim(32);
				break;
			}
		}
		else
		{
			switch (m_iMeleeStack)
			{
			case 0:
				m_pWeapon->Change_Anim(0);
				Change_Anim(0);
				break;
			case 1:
				m_pWeapon->Change_Anim(1);
				Change_Anim(1);
				break;
			case 2:
				m_pWeapon->Change_Anim(2);
				Change_Anim(2);
				break;
			case 3:
				m_pWeapon->Change_Anim(3);
				Change_Anim(3);
				break;
			case 4:
				m_pWeapon->Change_Anim(4);
				Change_Anim(4);
				break;
			}
		}
		};
	m_StateFunctions[PlayableState::MeleeAttack].OnState = [&](_float fTimeDelta) {
		if (m_bLastAnimFrameCalled) { Change_State(Idle2); }
		};
	m_StateFunctions[PlayableState::MeleeAttack].OnExitState = [&](_float fTimeDelta) {
		m_iMeleeStack = (m_iMeleeStack + 1) % 5;
		if (m_iNextState != MeleeAttack) m_iMeleeStack = 0;
		};

	//// 특수 일반공격
	m_StateFunctions[PlayableState::SpecialMelee].OnEnterState = [&](_float fTimeDelta) {
		m_StopWatchs[SW_Default].StartCheck();

		if (m_bUltimateMode)
		{
			if (m_iPrevState != SpecialMelee)
			{
				m_pWeapon->Change_Anim(34);
				Change_Anim(33);
			}
			else
			{
				m_pWeapon->Change_Anim(36);
				Change_Anim(35);
			}
		}
		else
		{
			if (moveVelocity.Length() > 0.1f)
			{
				m_pWeapon->Change_Anim(6);
				Change_Anim(6); // 이동하면서 검기
			}
			else
			{
				m_pWeapon->Change_Anim(7);
				Change_Anim(7);
			}
		}
		};
	m_StateFunctions[PlayableState::SpecialMelee].OnState = [&](_float fTimeDelta) {
		if (m_bUltimateMode)
		{
			// 시간 다되었을 때
			if (m_StopWatchs[SW_Default].fTime >= 3.5f)
			{
				m_iMeleeStack = 4;
				Change_State(MeleeAttack);
				m_StopWatchs[SW_Default].Clear();
			}
			else if (m_bLastAnimFrameCalled)
				Change_State(Idle2);
		}
		else
		{
			// 시간 다되었을 때
			if (m_StopWatchs[SW_Default].fTime >= 2.5f)
			{
				m_iMeleeStack = 4;
				Change_State(MeleeAttack);
				m_StopWatchs[SW_Default].Clear();
			}
			else if (m_bLastAnimFrameCalled)
				Change_State(Idle2);
		}
		};

#pragma endregion

#pragma region 피격

	m_StateFunctions[hit].OnEnterState = [&](_float fTimeDelta) {
		if (m_bUltimateMode) {
			m_pWeapon->Change_Anim(48);
			Change_Anim(52);
		}
		else {
			//m_pWeapon->Change_Anim(48);
			Change_Anim(53);
		}
	};
	m_StateFunctions[hit].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) { Change_State(Idle); } };
	
#pragma endregion

	return S_OK;
}

HRESULT Client::CPlayableObject::Add_Component(const INIT_DESC& initDesc)
{
	if(FAILED(__super::Add_Component(initDesc.ePropLevel, initDesc.eCreateLevel, initDesc.strModelKey)))
		return E_FAIL;

	// 충돌 콜라이더
	CBounding_Sphere::INIT_DESC sphereColDesc;
	sphereColDesc.vCenter = Vector3(0.f, 0.7f, 0.f);
	sphereColDesc.fRadius = 0.8f;

	CCollider::INIT_DESC colliderDesc;
	colliderDesc.eType = CCollider::TYPE_SPHERE;
	colliderDesc.iLayerType = Col_Player;
	colliderDesc.pInitDesc = make_shared<CBounding_Sphere::INIT_DESC>(sphereColDesc);
	colliderDesc.pOwner = shared_from_this();
	colliderDesc.iCreateLevelIndex = initDesc.eCreateLevel;
	m_MapColliderCom.emplace(L"Col_Body", CCollider::Create(m_pDevice, m_pContext, colliderDesc));

	// 일반공격 콜라이더
	sphereColDesc;
	sphereColDesc.vCenter = Vector3(0.f, 0.7f, 0.f);
	sphereColDesc.fRadius = 0.8f;

	colliderDesc;
	colliderDesc.eType = CCollider::TYPE_SPHERE;
	colliderDesc.iLayerType = Col_PlayerAttack;
	colliderDesc.pInitDesc = make_shared<CBounding_Sphere::INIT_DESC>(sphereColDesc);
	colliderDesc.pOwner = shared_from_this();
	colliderDesc.iCreateLevelIndex = initDesc.eCreateLevel;
	shared_ptr<CCollider> pAttackCol = CCollider::Create(m_pDevice, m_pContext, colliderDesc);
	pAttackCol->Set_Activate(false);
	m_MapColliderCom.emplace(L"Col_Attack", pAttackCol);

	return S_OK;
}

HRESULT Client::CPlayableObject::Add_Children(LEVEL eLevel)
{
	m_pWeapon = CWeapon::Create(m_pDevice, m_pContext, eLevel, L"E3AlphaMd010011P01 (merge)", m_pMeshCom->Get_Bone("Bip001Prop2"), MeshType::ANIM);
	if (nullptr == m_pWeapon)
		return E_FAIL;

	m_pWeapon->Get_Transform()->Rotation(Vector4(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_pWeapon->Get_Transform()->Rotation(Vector4(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	if(FAILED(__super::Add_Children(L"Weapon", m_pWeapon)))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::CPlayableObject::Bind_ShaderResources()
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

void Client::CPlayableObject::OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
{
	if (pCollider->Get_ColliderLayer() == Col_Monster)
	{
		if (pSrcCol == m_MapColliderCom[L"Col_Attack"])
		{
			static_pointer_cast<CMovableObject>(pCollider->Get_Owner().lock())->Hit(m_iAttackPower);
		}
	}	
}

void Client::CPlayableObject::OnCollisionStay(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
{
	if (pCollider->Get_ColliderLayer() == Col_Monster)
	{
		m_vRootMotionVelocity.x = 0.f;
		m_vRootMotionVelocity.z = 0.f;

		if (pSrcCol == m_MapColliderCom[L"Col_Body"])
		{
			Vector3 vDir = pCollider->Get_Center() - pSrcCol->Get_Center();
			_float fDist = vDir.Length();
			vDir.y = 0.f;
			vDir.Normalize();
			_float radiusAdd = pCollider->Get_Radius() + pSrcCol->Get_Radius();
			Vector3 vCurPos = m_pTransformCom->Get_Pos();
			m_pTransformCom->Set_Pos(vCurPos - vDir * (radiusAdd - fDist));

		}
	}
}

void Client::CPlayableObject::Move()
{
	if (moveVelocity.Length() > 0.1f)
	{
		if (m_iCurrentState == Idle || m_iCurrentState == Idle2 ||
			m_iCurrentState == Stop || m_iCurrentState == RunStartEnd ||
			(m_iCurrentState == Dodge) && m_dCurrentAnimFrame > 0.4)
		{
			Change_State(RunStart);
			moveVelocity = Vector3(0.f, 0.f, 0.f);
			return;
		}
		else if (m_iCurrentState >= CorePassiveStart &&
				 m_iCurrentState < PlayableStateEnd && 
				 m_iCurrentState != SpecialMelee)
		{
			moveVelocity = Vector3(0.f, 0.f, 0.f);
			
			return;
		}

		Vector3 vDir;

		vDir = AdjustVelocityWithCamera();
		
		if (m_iCurrentState != SpecialMelee)
		{
			Vector4 At = XMVectorSetW(m_pTransformCom->Get_Pos() + vDir, 1.f);
			m_pTransformCom->LookAt(At);
		}
	}
	else
	{		
		if (nullptr != m_pTarget.lock() &&
			m_iCurrentState >= CorePassiveStart &&
			m_iCurrentState <= SkillRedEX &&
			m_iCurrentState != CorePassiveAttack &&
			m_iMeleeStack != 3 &&
			m_dCurrentAnimFrame < 0.1)
		{
			Vector3 vDir = AdjustVelocityWithTarget();
			Vector4 At = XMVectorSetW(m_pTransformCom->Get_Pos() + vDir, 1.f);
			m_pTransformCom->LookAt(At);
		}
		
	}

}

Vector3 Client::CPlayableObject::AdjustVelocityWithCamera()
{
	Vector3 vDir;
	// 카메라가 보는 방향으로 진행하도록
	// 심플매스는 오른손좌표계라 Bakcward가져와야됨
	Matrix matCam = m_pGameInstance.lock()->Get_ViewMatrixInverse();
	Vector3 vCamRight, vCamLook;
	memcpy(&vCamRight, matCam.m[0], sizeof(Vector3));
	memcpy(&vCamLook, matCam.m[2], sizeof(Vector3));
	vCamRight.y = 0.f;
	vCamLook.y = 0.f;

	vCamRight.Normalize();
	vCamLook.Normalize();

	vCamRight *= moveVelocity.x;
	vCamLook *= moveVelocity.z;

	vDir = vCamRight + vCamLook;
	vDir.Normalize();
	Vector3 vSrc = m_pTransformCom->Get_Look();
	vSrc.y = 0.f;
	vSrc.Normalize();

	// 회전을 90도 이내로 할때의 보간
	//if (CMathUtils::RadFromDot(vSrc,vDir) < PI * 0.5f)
	if(vSrc.Dot(vDir) > 0.f) // 내적이 0보다 크면 예각 | 작으면 둔각
		vDir = CMathUtils::Slerp(vSrc, vDir, 0.3f);
	else
		vDir = CMathUtils::Slerp(vSrc, vDir, 0.8f);


	return vDir;
}

Vector3 Client::CPlayableObject::AdjustVelocityWithTarget()
{
	Vector3 vSrc = m_pTransformCom->Get_Look();
	Vector3 vDir = m_pTarget.lock()->Get_Transform()->Get_Pos() - m_pTransformCom->Get_Pos();

	vSrc.y = 0.f;
	vDir.y = 0.f;
	vSrc.Normalize();
	vDir.Normalize();

	if (vSrc.Dot(vDir) > 0.f) // 내적이 0보다 크면 예각 | 작으면 둔각
		vDir = CMathUtils::Slerp(vSrc, vDir, 0.3f);
	else
		vDir = CMathUtils::Slerp(vSrc, vDir, 0.8f);

	return vDir;
}

void Client::CPlayableObject::DetectNearestTarget()
{
	shared_ptr<CLayer> layer = m_pGameInstance.lock()->Get_Layer(L"Layer_Monster");
	if (layer == nullptr)
		return;

	_float fNearestDist = 20.f;
	Vector3 vPos = m_pTransformCom->Get_Pos();
	shared_ptr<CGameObject> tempTarget = nullptr;
	for (auto& GameObject : layer->Get_ObjList())
	{
		_float fDist = CMathUtils::GetDistance(GameObject->Get_Transform()->Get_Pos(), vPos);
		if (fDist < fNearestDist)
		{
			fDist = fNearestDist;
			tempTarget = GameObject;
		}
	}

	if (tempTarget == nullptr)
		return;

	m_pTarget = static_pointer_cast<CMovableObject>(tempTarget);
}

void Client::CPlayableObject::ComputeTarget()
{
	if (nullptr != m_pTarget.lock())
	{
		_float fTargetDist = CMathUtils::GetDistance(m_pTransformCom->Get_Pos(), m_pTarget.lock()->Get_Transform()->Get_Pos());
		if (fTargetDist >= 20.f)
			m_pTarget.reset();
	}
	else
	{
		DetectNearestTarget();
	}
}

void Client::CPlayableObject::Hit(_uint iDamage)
{

	if (m_iCurrentState == CorePassiveGauging && m_iParringStack < 3)
	{
		Change_State(CorePassiveParring);
		return;
	}

	Change_State(hit);
}

void Client::CPlayableObject::HardHit(_uint iDamage)
{
}

shared_ptr<CPlayableObject> Client::CPlayableObject::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc)
{
	shared_ptr<CPlayableObject> pInstance = make_shared<CPlayableObject>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(initDesc)))
	{
		MSG_BOX("Failed to Created : CPlayableObject");
		pInstance.reset();
	}

	return pInstance;
}