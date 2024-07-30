#include "pch.h"
#include "PlayableVera.h"
#include "Weapon.h"
#include "Layer.h"
#include "SkillBtnContainer.h"
#include "DefaultUI.h"
#include "Slider.h"
#include "UltimateSkillBtn.h"
#include "Camera.h"
#include "MeshEffect.h"
#include "ChosanEffect.h"
#include "ParticleEffect.h"
#include "FontUI.h"
#include "DistortionPointRect.h"

Client::CPlayableVera::CPlayableVera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CMovableObject(pDevice, pContext)
{
	m_StateFunctions.resize(PlayableStateEnd);
}

void Client::CPlayableVera::UseSkill(SkillType eSkillColor, _int iChain)
{
	SkillBallChain Skill;
	Skill.eSkill = eSkillColor;
	Skill.iChain = iChain;
	m_SkillInputQueue.push(Skill);

	if (iChain == 3)
		m_eLastTripleSkillball = eSkillColor;
	else
		m_eLastTripleSkillball = SKILL_NONE;
}

_bool Client::CPlayableVera::UseUltimateSkill()
{
	if (m_iCurrentSp < 80)
		return false;

	SkillBallChain Skill;
	Skill.eSkill = SkillType::ULTIMATE;
	Skill.iChain = 3;

	m_iCurrentSp -= 80;
	m_SkillInputQueue.push(Skill);
	m_pSpSlider->SetSlideRate(Get_SpRate());

	return true;
}

void Client::CPlayableVera::PlayWinMotion()
{
	Change_State(Win);
}

void Client::CPlayableVera::PlayBornSound()
{
	PlaySoundOnce(L"c_weilaRK3_act_born", 0.5f);
	PlaySoundOnce(L"Born", 0.5f);
}

HRESULT Client::CPlayableVera::Initialize(const INIT_DESC& initDesc)
{
	if (FAILED(__super::Initialize(initDesc.vInitPos)))
		return E_FAIL;

	m_iMaxHp = m_iCurrentHp = initDesc.iHp;
	m_iAttackPower = initDesc.iAttackPower;

	if (FAILED(Add_StateFunction()))
		return E_FAIL;

	if (FAILED(Add_Component(initDesc))) return E_FAIL;

	if (FAILED(Add_Children(initDesc.ePropLevel))) return E_FAIL;
	
	if (FAILED(Load_MeshEffects(initDesc.ePropLevel, initDesc.eCreateLevel, L"PlayerEffect", &g_TimeScale_Player, L"../Bin/EffectPrefab/MeshEffect/Vera")))
		return E_FAIL;

	if (FAILED(Load_ParticleEffects(initDesc.ePropLevel, initDesc.eCreateLevel, L"PlayerEffect", &g_TimeScale_Player, L"../Bin/EffectPrefab/ModelParticleList/R3WeilaMd010021 (merge).json")))
		return E_FAIL;

	m_eCurrentLevel = initDesc.eCreateLevel;

	Change_State(EnterRoom);

	if (FAILED(Init_UI(initDesc)))
		return E_FAIL;

	m_StopWatchs[SW_FOOTSTEP].StartCheck();

	return S_OK;
}

void Client::CPlayableVera::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (nullptr != m_pTarget.lock() && m_pTarget.lock()->isDead())
		m_pTarget.reset();

	if (nullptr != m_pTarget.lock())
	{
		if (!m_pTargetHp->isActivate())
		{
			m_pTargetHpBg->Activate();
			m_pTargetHp->Activate();

			m_pTargetNameFont->Set_Text(m_pTarget.lock()->Get_Name());
			m_pTargetNameFont->Activate();
		}
		m_pTargetHp->SetSlideRate(m_pTarget.lock()->Get_HpRate());
	}
	else
	{
		if (m_pTargetHpBg->isActivate())
		{
			m_pTargetHpBg->InActivate();
			m_pTargetHp->InActivate();
			m_pTargetNameFont->InActivate();
		}
	}

	if (false == m_bCanSuperDodge)
	{
		if (m_StopWatchs[SW_DODGE].fTime >= m_fSuperDodgeCool)
		{
			m_bCanSuperDodge = true;
			m_StopWatchs[SW_DODGE].Clear();
		}
	}

	if (m_bOnSuperDodgeTime)
	{
		if (m_StopWatchs[SW_TIMESTOP].fTime >= m_fSuperDodgeTime)
		{
			m_bOnSuperDodgeTime = false;
			m_StopWatchs[SW_TIMESTOP].Clear();
			g_TimeScale_Monster = 1.f;
		}
	}
}

_int Client::CPlayableVera::Tick(_float fTimeDelta)
{
	m_pMeshCom->Play_Animation(fTimeDelta, m_vRootMotionVelocity, m_bLastAnimFrameCalled, m_dCurrentAnimFrame,true, true, true);

	if (m_iCurrentState >= RunStart && m_iCurrentState <= RunL)
		m_vRootMotionVelocity = Vector3(0.f, 0.f, 0.f);

	SkillBallQueueCheck();

	__super::Tick(fTimeDelta);

	Move();

	// 최종moveVelocity 속도 * look방향 transform이동시킴
	m_pTransformCom->Go_Straight(moveVelocity.Length() * fTimeDelta, m_pNavigationCom);

	if (moveVelocity.Length() > 0.1f)
	{
		if (m_StopWatchs[SW_FOOTSTEP].fTime >= 0.3f)
		{
			m_StopWatchs[SW_FOOTSTEP].fTime = 0.f;
			PlaySoundOnce(L"footstepAsphalt_s1", 0.5f);
		}
	}

	if (m_vRootMotionVelocity.Length() > 0.1f)
	{
		m_pTransformCom->Go_Right(m_vRootMotionVelocity.x, m_pNavigationCom);
		m_pTransformCom->Go_Straight(-m_vRootMotionVelocity.z, m_pNavigationCom);
	}

	_float y = m_pNavigationCom->Get_HeightOnNav(m_pTransformCom->Get_Pos());
	m_pTransformCom->Set_Y(y);

	PlaySound();

	return OBJ_ALIVE;
}

void Client::CPlayableVera::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_bKillRender)
		return;

	if (FAILED(m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;
	if (FAILED(m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_SHADOW, shared_from_this())))
		return;
}

HRESULT Client::CPlayableVera::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//for (_uint i = 0; i < m_pMeshCom->Get_MeshCount(); ++i)
	//{
	//	// 텍스처 바인딩
	//	if (FAILED(m_pMeshCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
	//		return E_FAIL;
	//	if (FAILED(m_pMeshCom->Bind_BoneMatrices("g_BoneMatrices", i)))
	//		return E_FAIL;

	//	m_pMeshCom->Render(i, 1);
	//}

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

HRESULT Client::CPlayableVera::Render_Shadow()
{
	m_pWeaponDown->Render_Shadow();
	m_pWeaponUpper->Render_Shadow();

	Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	Matrix ViewMatrix = *(m_pGameInstance.lock()->Get_LightViewMatrix(0));

	Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

	if (FAILED(m_pMeshCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pMeshCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pMeshCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	for (_uint i = 0; i < m_pMeshCom->Get_MeshCount(); ++i)
	{
		// 텍스처 바인딩
		if (FAILED(m_pMeshCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pMeshCom->Bind_BoneMatrices("g_BoneMatrices", i)))
			return E_FAIL;

		m_pMeshCom->Render(i, 1);
	}

	return S_OK;
}

void Client::CPlayableVera::KeyInput()
{
	if (m_iCurrentState == SkillUltimate)
		return;

	if (m_pGameInstance.lock()->GetMouseButton(MOUSEKEYSTATE::DIM_LB) && nullptr != m_pTarget.lock())
	{
		if (m_pGameInstance.lock()->Get_DIMouseMove(DIMS_X) != 0)
			m_pTarget.reset();
	}

	InputMovement();
	InputDodge();
	InputAttack();

	moveVelocity.Normalize();
	moveVelocity *= m_fSpeed;

	if (m_iCurrentSp > 80)
		m_pUltimateBtn->Activate();

#ifdef _DEBUG
	if (m_pGameInstance.lock()->GetKeyDown(DIK_O))
		m_iCurrentSp = m_iMaxSp;
#endif // _DEBUG

}

void Client::CPlayableVera::SkillBallQueueCheck()
{
	if (false == m_SkillInputQueue.empty())
	{
		if ((m_iCurrentState < SkillBlue || m_iCurrentState > SkillUltimate) ||
			(m_iCurrentState >= SkillBlue && m_iCurrentState <= SkillUltimate && m_dCurrentAnimFrame > 0.3))
		{
			_bool isTriple = m_SkillInputQueue.front().iChain == 3;

			switch (m_SkillInputQueue.front().eSkill)
			{
			case SkillType::BLUE:
				m_SkillInputQueue.pop();
				Change_State(SkillBlue);
				if (isTriple)
					PlaySoundOnce(L"Vera_BlueTriple", 0.5f);
				break;
			case SkillType::RED:
				m_SkillInputQueue.pop();
				Change_State(SkillRed);
				if (isTriple)
					PlaySoundOnce(L"Vera_RedTriple", 0.5f);
				break;
			case SkillType::YELLOW:
				m_SkillInputQueue.pop();
				Change_State(SkillYellow);
				if (isTriple)
					PlaySoundOnce(L"Vera_YellowTriple", 0.5f);
				break;
			case SkillType::ULTIMATE:
				m_SkillInputQueue.pop();
				Change_State(SkillUltimate);
				if (isTriple)
					PlaySoundOnce(L"Vera_UltimateTalk", 0.5f);
				break;
			}

			if (nullptr == m_pTarget.lock())
				DetectNearestTarget();
		}
	}
}

void Client::CPlayableVera::InputDodge()
{
	if (m_pGameInstance.lock()->GetMouseButtonDown(DIM_RB))
	{
		if (m_iCurrentState >= Dodge && m_dCurrentAnimFrame > 0.26)
			Change_State(Dodge);
		else if (m_iCurrentState < Dodge)
			Change_State(Dodge);
	}

}

void Client::CPlayableVera::InputAttack()
{
	if (m_pGameInstance.lock()->GetKeyDown(DIK_SPACE))
	{
		m_StopWatchs[SW_0].StartCheck();

		if (nullptr == m_pTarget.lock())
			DetectNearestTarget();
	}

	if (m_pGameInstance.lock()->GetKey(DIK_SPACE))
	{
		if (m_iCorePassiveStack > 0)
		{
			if (m_StopWatchs[SW_0].fTime > 0.4f)
			{
				Change_State(CorePassive);
				UseCorePassive();
				m_StopWatchs[SW_0].Clear();
				return;
			}
		}
	}

	if (m_pGameInstance.lock()->GetKeyUp(DIK_SPACE))
	{
		if (m_iCurrentState == CorePassive)
			return;

		if ((m_iCurrentState >= Idle && m_iCurrentState <= Stop) ||
			(m_iCurrentState >= SkillBlue && m_iCurrentState <= SkillRed))
		{
			switch (m_eLastTripleSkillball)
			{
			case RED:
				Change_State(SkillRedEX);
				AddCorePassiveStack();
				break;
			case BLUE:
				Change_State(SkillBlueEX);
				AddCorePassiveStack();
				break;
			case YELLOW:
				Change_State(SkillYellowEX);
				AddCorePassiveStack();
				break;
			default:
				Change_State(MeleeAttack);
				break;
			}
		}
		else
		{
			switch (m_iCurrentState)
			{
			case MeleeAttack:
				if (m_iMeleeStack <= 5 && m_dCurrentAnimFrame > 0.17)
					Change_State(MeleeAttack);
				break;

			case SkillRedEX:
				m_iMeleeStack = 4;
				Change_State(MeleeAttack);
				break;

			case SkillYellowEX:
			case SkillBlueEX:
				m_iMeleeStack = 2;
				Change_State(MeleeAttack);
				break;
			}
		}
		
		m_StopWatchs[SW_0].Clear();
	}
}

void Client::CPlayableVera::InputMovement()
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

HRESULT Client::CPlayableVera::Init_UI(const INIT_DESC& initDesc)
{
	// 스킬볼 컨테이너
	CSkillBtnContainer::INIT_DESC desc;
	desc.eCreateLevel = initDesc.eCreateLevel;
	desc.vPos = Vector2(900.f, 520.f);
	m_pSkillBallContainer = CSkillBtnContainer::Create(m_pDevice, m_pContext, desc, static_pointer_cast<CPlayableVera>(shared_from_this()));
	if (nullptr == m_pSkillBallContainer)
		return E_FAIL;

	// 체력바 베이스
	CUIBase::INIT_DESC InitialDesc;
	InitialDesc.strTexKey = L"DefaultStatusUI";
	InitialDesc.vPos = Vector2(150.f, 40.f);
	InitialDesc.vSize = Vector2(300.f, 60.f);

	shared_ptr<CDefaultUI> pTextUI = CDefaultUI::Create(m_pDevice, m_pContext, InitialDesc);
	m_pGameInstance.lock()->Add_UI(initDesc.eCreateLevel, pTextUI);

	
	// 체력바
	CSlider::INIT_DESC sliderDesc;
	sliderDesc.strTexKey = L"FightSelfHpBg";
	sliderDesc.vPos = Vector2(174.f, 53.f);
	sliderDesc.vSize = Vector2(185.f, 9.5f);
	sliderDesc.vColor = Vector4(1.f, 1.f, 1.f, 1.f);

	m_pHpSlider = CSlider::Create(m_pDevice, m_pContext, sliderDesc);
	m_pHpSlider->SetSlideRate(Get_HpRate());
	m_pGameInstance.lock()->Add_UI(initDesc.eCreateLevel, m_pHpSlider);

	// sp게이지
	sliderDesc.strTexKey = L"FightSelfHpBg";
	sliderDesc.vPos = Vector2(168.f, 67.5f);
	sliderDesc.vSize = Vector2(172.f, 4.1f);
	sliderDesc.vColor = Vector4(0.5f, 0.57f, 0.9f, 1.f);

	m_pSpSlider = CSlider::Create(m_pDevice, m_pContext, sliderDesc);
	m_pSpSlider->SetSlideRate(Get_SpRate());
	m_pGameInstance.lock()->Add_UI(initDesc.eCreateLevel, m_pSpSlider);

	// 필살기 UI
	InitialDesc.strTexKey = L"FightSkillIconR3Weila51";
	InitialDesc.vPos = Vector2(1100.f, 650.f);
	InitialDesc.vSize = Vector2(100.f, 100.f);
	m_pUltimateBtn = CUltimateSkillBtn::Create(m_pDevice, m_pContext, InitialDesc);
	m_pUltimateBtn->Set_Player(static_pointer_cast<CPlayableVera>(shared_from_this()));
	m_pUltimateBtn->InActivate();
	m_pGameInstance.lock()->Add_UI(initDesc.eCreateLevel, m_pUltimateBtn);

	// 코패 에너지UI
	shared_ptr<CDefaultUI> pBgUI = nullptr;
	CDefaultUI::INIT_DESC defaultUIDesc;
	defaultUIDesc.vSize = Vector2(50.f, 50.f);
	defaultUIDesc.vPos = Vector2(440.f, 650.f);
	for (_int iEnergyUICnt = 0; iEnergyUICnt < sizeof(m_pEneryUIs) / sizeof(m_pEneryUIs[0]); ++iEnergyUICnt)
	{
		defaultUIDesc.vPos.x += 100.f;

		// 배경깔기
		defaultUIDesc.strTexKey = L"BgEnergyR3Weila";
		pBgUI = CDefaultUI::Create(m_pDevice, m_pContext, defaultUIDesc);
		m_pGameInstance.lock()->Add_UI(initDesc.eCreateLevel, pBgUI);

		// 색들어간UI
		defaultUIDesc.strTexKey = L"EnergyR3Weila";
		m_pEneryUIs[iEnergyUICnt] = CDefaultUI::Create(m_pDevice, m_pContext, defaultUIDesc);
		m_pEneryUIs[iEnergyUICnt]->InActivate();
		m_pGameInstance.lock()->Add_UI(initDesc.eCreateLevel, m_pEneryUIs[iEnergyUICnt]);
	}

	// 대상 몬스터 체력UI
	defaultUIDesc.vPos = Vector2(640.f, 67.5f);
	defaultUIDesc.vSize = Vector2(400.f, 7.f);
	defaultUIDesc.strTexKey = L"FightSelfHpBg";

	m_pTargetHpBg = CDefaultUI::Create(m_pDevice, m_pContext, defaultUIDesc);
	m_pTargetHpBg->InActivate();
	m_pGameInstance.lock()->Add_UI(initDesc.eCreateLevel, m_pTargetHpBg);

	sliderDesc.strTexKey = L"FightSelfHpBg";
	sliderDesc.vPos = Vector2(640.f, 67.5f);
	sliderDesc.vSize = Vector2(400.f, 7.f);
	sliderDesc.vColor = Vector4(1.f, 1.f, 1.f, 1.f);
	m_pTargetHp = CSlider::Create(m_pDevice, m_pContext, sliderDesc);
	m_pTargetHp->InActivate();
	m_pGameInstance.lock()->Add_UI(initDesc.eCreateLevel, m_pTargetHp);

	// 대상 몬스터 이름UI
	CFontUI::INIT_DESC fontDesc;
	fontDesc.fScale = 0.5f;
	fontDesc.strFontTag = L"Font_Default";
	fontDesc.vColor = Vector4(1.f, 1.f, 1.f, 1.f);
	fontDesc.vPosition = Vector2(620.f, 30.f);

	m_pTargetNameFont = CFontUI::Create(m_pDevice, m_pContext, fontDesc);
	m_pGameInstance.lock()->Add_UI(initDesc.eCreateLevel, m_pTargetNameFont);

	return S_OK;
}

void Client::CPlayableVera::PlaySound()
{
	_int iCurrentKeyFrame = m_pMeshCom->Get_CurrentKeyFrame();

	switch (m_iCurrentState)
	{
	case MeleeAttack:
		if (m_iMeleeStack == 1)
		{
			if (iCurrentKeyFrame == 5)
				PlaySoundOnce(L"c_weilaRK3_atk_nm_2_2", 0.5f);
		}
		else if (m_iMeleeStack == 3)
		{
			if(iCurrentKeyFrame == 9)
				PlaySoundOnce(L"c_weilaRK3_atk_nm_4_2", 0.5f);
			//else if(iCurrentKeyFrame == 12)
			//	PlaySoundOnce(L"c_weilaRK3_atk_nm_4_3", 0.5f);
		}
		else if (m_iMeleeStack == 4)
		{
			if(iCurrentKeyFrame == 10)
				PlaySoundOnce(L"c_weilaRK3_atk_nm_5_2", 0.5f);
		}
		break;
	case SkillRed:
		if (iCurrentKeyFrame == 1)
			PlaySoundOnce(L"c_weilaRK3_skill_red_1", 0.5f);
		else if (iCurrentKeyFrame == 9)
			PlaySoundOnce(L"c_weilaRK3_skill_red_2", 0.5f);
		else if (iCurrentKeyFrame == 15)
			PlaySoundOnce(L"c_weilaRK3_skill_red_4", 0.5f);
		break;
	case SkillYellowEX:
		if(iCurrentKeyFrame == 9)
			PlaySoundOnce(L"c_weilaRK3_skill_yellow_ex_2", 0.5f);
		break;
	case SkillYellow:
		if(iCurrentKeyFrame == 5)
			PlaySoundOnce(L"c_weilaRK3_skill_yellow_2", 0.5f);
		else if(iCurrentKeyFrame == 18)
			PlaySoundOnce(L"c_weilaRK3_skill_yellow_3", 0.5f);
		break;
	case SkillBlueEX:
		if(iCurrentKeyFrame == 8)
			PlaySoundOnce(L"c_weilaRK3_skill_blue_ex_2", 0.5f);
		break;
	case CorePassive:
		if(iCurrentKeyFrame == 15)
			PlaySoundOnce(L"c_weilaRK3_skill_ex_luolei_2", 0.5f);
		break;
	case SkillUltimate:
		if(iCurrentKeyFrame == 80)
			PlaySoundOnce(L"c_weilaRK3_EG_skill_ultimate_back", 0.5f);
		break;
	}
}

HRESULT Client::CPlayableVera::Add_StateFunction()
{	
#pragma region 연출모션 
	m_StateFunctions[EnterRoom].OnEnterState = [&](_float fTimeDelta) {
		Change_Anim(39);
		};
	m_StateFunctions[EnterRoom].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) { Change_State(Idle2); } };

	m_StateFunctions[Win].OnEnterState = [&](_float fTimeDelta) {
		PlaySoundOnce(L"c_weilaRK3_act_win", 0.5f);
		PlaySoundOnce(L"Vera_WinTalk", 0.5f);
		Change_Anim(65); };
#pragma endregion

#pragma region 이동관련

	// 달리기 직후
	m_StateFunctions[PlayableState::RunStart].OnEnterState = [&](_float fTimeDelta) { Change_Anim(54); };
	m_StateFunctions[PlayableState::RunStart].OnState = [&](_float fTimeDelta) {
		if (m_bLastAnimFrameCalled) { Change_State(Run); }
		else if (moveVelocity.Length() < 0.1f) { Change_State(RunStartEnd); }
		};

	// 달리기 직후 이전에 멈출때
	m_StateFunctions[PlayableState::RunStartEnd].OnEnterState = [&](_float fTimeDelta) { Change_Anim(55); };
	m_StateFunctions[PlayableState::RunStartEnd].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) { Change_State(Idle2); }  };

	// 달리다가 멈추기 상태
	m_StateFunctions[PlayableState::Stop].OnEnterState = [&](_float fTimeDelta) {
		Change_Anim(62);};
	m_StateFunctions[PlayableState::Stop].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) {Change_State(Idle2); }  };

	// 정지모션 2상태
	m_StateFunctions[PlayableState::Idle2].OnEnterState = [&](_float fTimeDelta) {
		m_StopWatchs[SW_Default].StartCheck();
		Change_Anim(59);
		};
	m_StateFunctions[PlayableState::Idle2].OnState = [&](_float fTimeDelta) { if (m_StopWatchs[SW_Default].fTime >= 4.f) { Change_State(IdleChange); } };
	m_StateFunctions[PlayableState::Idle2].OnExitState = [&](_float fTimeDelta) { m_StopWatchs[SW_Default].Clear(); };
	
	// 정지모션 변경상태
	m_StateFunctions[PlayableState::IdleChange].OnEnterState = [&](_float fTimeDelta) {  Change_Anim(60); };
	m_StateFunctions[PlayableState::IdleChange].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) { Change_State(Idle); } };

	// 정지모션 1 상태
	m_StateFunctions[PlayableState::Idle].OnEnterState = [&](_float fTimeDelta) { Change_Anim(58); };

	// 달리기 상태
	m_StateFunctions[PlayableState::Run].OnEnterState = [&](_float fTimeDelta){ 
		Change_Anim(51);};
	m_StateFunctions[PlayableState::Run].OnState = [&](_float fTimeDelta){ if (moveVelocity.Length() < 0.1f) { Change_State(Stop); } };

	// 오른쪽달리기
	m_StateFunctions[PlayableState::RunR].OnEnterState = [&](_float fTimeDelta){ Change_Anim(53);};

	// 왼쪽달리기
	m_StateFunctions[PlayableState::RunL].OnEnterState = [&](_float fTimeDelta){Change_Anim(52);};

	// 회피
	m_StateFunctions[PlayableState::Dodge].OnEnterState = [&](_float fTimeDelta) {
		_bool DodgeForward = false;
		m_pGameInstance.lock()->Play_Sound(L"Dodge", 0.5f);
		if (moveVelocity.Length() > 0.1f)
			DodgeForward = true;
		
			if (DodgeForward) {
				Change_Anim(49);
			}
			else {
				Change_Anim(50);
			} 
		};
	m_StateFunctions[PlayableState::Dodge].OnState = [&](_float fTimeDelta) { 
		if (m_bLastAnimFrameCalled) 
		{
				Change_State(Idle2);
		}
		};


#pragma endregion

#pragma region 코어 패시브

	// 코어 패시브 공격
	m_StateFunctions[PlayableState::CorePassive].OnEnterState = [&](_float fTimeDelta) {
		PlaySoundOnce(L"c_weilaRK3_skill_ex_luolei_1", 0.5f);
		Change_Anim(22);};
	m_StateFunctions[PlayableState::CorePassive].OnState = [&](_float fTimeDelta) {if (m_bLastAnimFrameCalled) { Change_State(Idle2); }};

#pragma endregion

#pragma region 스킬
	// 궁극기
	m_StateFunctions[PlayableState::SkillUltimate].OnEnterState = [&](_float fTimeDelta) {
		Change_Anim(25);
		g_TimeScale_Monster = 0.01f;
		m_pGameInstance.lock()->Set_MainCamera(m_eCurrentLevel, L"Player_Ultimate");
		PlaySoundOnce(L"c_weilaRK3_skill_ultimate", 0.5f);
		m_pWeaponDown->Play_Trail();
		};
	m_StateFunctions[PlayableState::SkillUltimate].OnState = [&](_float fTimeDelta) {
		if (m_bLastAnimFrameCalled) { Change_State(Idle2); }
		if (m_pMeshCom->Get_CurrentKeyFrame() == 10)
		{
			m_pUltimateDistortEffect->Get_Transform()->Set_Pos(m_vMyPos + m_vMyLook * 1.25f);
			m_pUltimateDistortEffect->PlayOnce();
		}
		};
	m_StateFunctions[PlayableState::SkillUltimate].OnExitState = [&](_float fTimeDelta) { 
		g_TimeScale_Monster = 1.f;
		m_pWeaponDown->Stop_Trail();
		};

	// 파랑 볼
	m_StateFunctions[PlayableState::SkillBlue].OnEnterState = [&](_float fTimeDelta) { 
		Change_Anim(11);
		PlaySoundOnce(L"c_weilaRK3_skill_blue_2", 0.5f);
		};
	m_StateFunctions[PlayableState::SkillBlue].OnState = [&](_float fTimeDelta) {if(m_bLastAnimFrameCalled) { Change_State(Idle2); }};

	// 파랑랑 볼Ex -> 이후 평타시 특수평타로
	m_StateFunctions[PlayableState::SkillBlueEX].OnEnterState = [&](_float fTimeDelta) { Change_Anim(12); 
		PlaySoundOnce(L"c_weilaRK3_skill_blue_ex_1", 0.5f);
		};
	m_StateFunctions[PlayableState::SkillBlueEX].OnState = [&](_float fTimeDelta) {if (m_bLastAnimFrameCalled) { Change_State(Idle2); }};

	// 빨강 볼 (3체인 이후 일반공격시 발동하는 추가타)
	m_StateFunctions[PlayableState::SkillRed].OnEnterState = [&](_float fTimeDelta) {Change_Anim(7); };
	m_StateFunctions[PlayableState::SkillRed].OnState = [&](_float fTimeDelta) {if (m_bLastAnimFrameCalled) { Change_State(Idle2); }};

	// 빩랑 볼Ex -> 이후 평타시 특수평타로
	m_StateFunctions[PlayableState::SkillRedEX].OnEnterState = [&](_float fTimeDelta) {
		Change_Anim(8); 
		PlaySoundOnce(L"c_weilaRK3_skill_red_ex", 0.5f);
		};
	m_StateFunctions[PlayableState::SkillRedEX].OnState = [&](_float fTimeDelta) {if (m_bLastAnimFrameCalled) { Change_State(Idle2); }};

	// 노랑 볼
	m_StateFunctions[PlayableState::SkillYellow].OnEnterState = [&](_float fTimeDelta) { 
		Change_Anim(19);
		PlaySoundOnce(L"c_weilaRK3_skill_yellow_1", 0.5f);
		};
	m_StateFunctions[PlayableState::SkillYellow].OnState = [&](_float fTimeDelta) {if (m_bLastAnimFrameCalled) { Change_State(Idle2); }};

	// 노랑 볼Ex (3체인 이후 일반공격시 발동하는 추가타) -> 이후 평타시 특수평타로
	m_StateFunctions[PlayableState::SkillYellowEX].OnEnterState = [&](_float fTimeDelta) { 
		Change_Anim(20); 
		PlaySoundOnce(L"c_weilaRK3_skill_yellow_ex_1", 0.5f);
		};
	m_StateFunctions[PlayableState::SkillYellowEX].OnState = [&](_float fTimeDelta) {if (m_bLastAnimFrameCalled) { Change_State(Idle2); }};

#pragma endregion

#pragma region 평타

		// 일반공격
	m_StateFunctions[PlayableState::MeleeAttack].OnEnterState = [&](_float fTimeDelta) {
			switch (m_iMeleeStack)
			{
			case 0:
				Change_Anim(0);
				PlaySoundOnce(L"c_weilaRK3_atk_nm_1", 0.5f);
				break;
			case 1:
				Change_Anim(1);
				PlaySoundOnce(L"c_weilaRK3_atk_nm_2_1", 0.5f);
				break;
			case 2:
				Change_Anim(2);
				PlaySoundOnce(L"c_weilaRK3_atk_nm_3", 0.5f);
				break;
			case 3:
				Change_Anim(3);
				PlaySoundOnce(L"c_weilaRK3_atk_nm_4_1", 0.5f);
				break;
			case 4:
				Change_Anim(4);
				PlaySoundOnce(L"c_weilaRK3_atk_nm_5_1", 0.5f);
				break;
			case 5:
				Change_Anim(5);
				PlaySoundOnce(L"c_weilaRK3_atk_nm_6_dilie", 0.5f);
				break;
			}
	};
	m_StateFunctions[PlayableState::MeleeAttack].OnState = [&](_float fTimeDelta) {if (m_bLastAnimFrameCalled) { Change_State(Idle2); }};
	m_StateFunctions[PlayableState::MeleeAttack].OnExitState = [&](_float fTimeDelta) {
		m_iMeleeStack = (m_iMeleeStack + 1) % 6;
		if (m_iNextState != MeleeAttack) m_iMeleeStack = 0;
		resetSoundInfo();
		};

#pragma endregion

#pragma region 피격

	m_StateFunctions[hit].OnEnterState = [&](_float fTimeDelta) {Change_Anim(42);};
	m_StateFunctions[hit].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) { Change_State(Idle2); } };
	
#pragma endregion

	return S_OK;
}
 
HRESULT Client::CPlayableVera::Add_Component(const INIT_DESC& initDesc)
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

	// 일반공격 콜라이더1
	sphereColDesc;
	sphereColDesc.vCenter = Vector3(0.f, 0.9f, 1.5f);
	sphereColDesc.fRadius = 1.6f;

	colliderDesc;
	colliderDesc.eType = CCollider::TYPE_SPHERE;
	colliderDesc.iLayerType = Col_PlayerAttack;
	colliderDesc.pInitDesc = make_shared<CBounding_Sphere::INIT_DESC>(sphereColDesc);
	colliderDesc.pOwner = shared_from_this();
	colliderDesc.iCreateLevelIndex = initDesc.eCreateLevel;
	shared_ptr<CCollider> pAttackCol = CCollider::Create(m_pDevice, m_pContext, colliderDesc);
	pAttackCol->Set_Activate(false);
	m_MapColliderCom.emplace(L"Col_Attack1", pAttackCol);

	// 일반공격 콜라이더2
	sphereColDesc;
	sphereColDesc.vCenter = Vector3(0.f, 0.9f, 0.f);
	sphereColDesc.fRadius = 2.5f;

	colliderDesc;
	colliderDesc.eType = CCollider::TYPE_SPHERE;
	colliderDesc.iLayerType = Col_PlayerAttack;
	colliderDesc.pInitDesc = make_shared<CBounding_Sphere::INIT_DESC>(sphereColDesc);
	colliderDesc.pOwner = shared_from_this();
	colliderDesc.iCreateLevelIndex = initDesc.eCreateLevel;
	pAttackCol = CCollider::Create(m_pDevice, m_pContext, colliderDesc);
	pAttackCol->Set_Activate(false);
	m_MapColliderCom.emplace(L"Col_Attack2", pAttackCol);

	// 초산회피 콜라이더
	sphereColDesc.vCenter = Vector3(0.f, 0.9f, 0.f);
	sphereColDesc.fRadius = 4.5f;

	colliderDesc;
	colliderDesc.eType = CCollider::TYPE_SPHERE;
	colliderDesc.iLayerType = Col_PlayerDodge;
	colliderDesc.pInitDesc = make_shared<CBounding_Sphere::INIT_DESC>(sphereColDesc);
	colliderDesc.pOwner = shared_from_this();
	colliderDesc.iCreateLevelIndex = initDesc.eCreateLevel;
	pAttackCol = CCollider::Create(m_pDevice, m_pContext, colliderDesc);
	pAttackCol->Set_Activate(false);
	m_MapColliderCom.emplace(L"Col_Dodge", pAttackCol);

	// 일반공격 콜라이더3
	CBounding_OBB::INIT_DESC obbDesc;
	obbDesc;
	obbDesc.vCenter = Vector3(0.f, 1.3f, 10.f);
	obbDesc.vExtents = Vector3(1.f,1.f,10.f);
	obbDesc.vRadians = Vector3(0.f, 0.f, 0.f);

	colliderDesc;
	colliderDesc.eType = CCollider::TYPE_OBB;
	colliderDesc.iLayerType = Col_PlayerAttack;
	colliderDesc.pInitDesc = make_shared<CBounding_OBB::INIT_DESC>(obbDesc);
	colliderDesc.pOwner = shared_from_this();
	colliderDesc.iCreateLevelIndex = initDesc.eCreateLevel;
	pAttackCol = CCollider::Create(m_pDevice, m_pContext, colliderDesc);
	pAttackCol->Set_Activate(false);
	m_MapColliderCom.emplace(L"Col_Attack3", pAttackCol);

	return S_OK;
}

HRESULT Client::CPlayableVera::Add_Children(LEVEL eLevel)
{
	m_pWeaponUpper = CWeapon::Create(m_pDevice, m_pContext, eLevel, L"E3LanceUpper", m_pMeshCom->Get_Bone("WeaponCase2"),MeshType::NON_ANIM);
	if (nullptr == m_pWeaponUpper)
		return E_FAIL;
	m_pWeaponDown = CWeapon::Create(m_pDevice, m_pContext, eLevel, L"E3LanceDown", m_pMeshCom->Get_Bone("WeaponCase1"), MeshType::NON_ANIM, true);
	if (nullptr == m_pWeaponDown)
		return E_FAIL;

	m_pWeaponUpper->Get_Transform()->Rotation(Vector4(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_pWeaponUpper->Get_Transform()->Rotation(Vector4(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	m_pWeaponUpper->Get_Transform()->Go_Up(-1.5f);
	m_pWeaponDown->Get_Transform()->Rotation(Vector4(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_pWeaponDown->Get_Transform()->Rotation(Vector4(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	m_pWeaponDown->Get_Transform()->Go_Up(-1.f);

	if(FAILED(__super::Add_Children(L"WeaponUpper", m_pWeaponUpper)))
		return E_FAIL;
	if(FAILED(__super::Add_Children(L"WeaponDown", m_pWeaponDown)))
		return E_FAIL;

	// 크랙 이펙트 생성
	CMeshEffect::INIT_DESC effectDesc = {};
	effectDesc.eLevel = eLevel;
	effectDesc.FXPrefabPath = L"../Bin/EffectPrefab/MeshEffect/Vera/Zheng.MeshFX";
	m_pGroundCrack = CMeshEffect::Create(m_pDevice, m_pContext, effectDesc);
	if (FAILED(m_pGameInstance.lock()->Add_Object(eLevel, L"Layer_PlayerEffect", m_pGroundCrack, &g_TimeScale_Player)))
	{
		MSG_BOX("CPlayableVera::Add_Children");
		return E_FAIL;
	}

	// 초산 이펙트 생성
	CChosanEffect::INIT_DESC ChosanDesc;
	ChosanDesc.eLevel = eLevel;
	ChosanDesc.fStartScale = 1.f;
	ChosanDesc.fEndScale = 30.f;
	ChosanDesc.fMoveSpeed = 25.f;
	ChosanDesc.strEffectBufferPath = L"../Bin/Resources/Effects/UiFightBabelTower01.AnimFbx";

	m_pChosanEffect = CChosanEffect::Create(m_pDevice, m_pContext, ChosanDesc);
	
	if (FAILED(m_pGameInstance.lock()->Add_Object(eLevel, L"Layer_PlayerEffect", m_pChosanEffect, &g_TimeScale_Player)))
	{
		MSG_BOX("CPlayableVera::Add_Children");
		return E_FAIL;
	}

	// 피격 이펙트 생성
	CParticleEffect::INIT_DESC particleDesc;
	particleDesc.eLevel = eLevel;
	particleDesc.FXPrefabPath = L"../Bin/EffectPrefab/ParticleEffect/VeraHit.Particle";
	m_pHitParticle = CParticleEffect::Create(m_pDevice, m_pContext, particleDesc);
	if (FAILED(m_pGameInstance.lock()->Add_Object(eLevel, L"Layer_PlayerEffect", m_pHitParticle, &g_TimeScale_Player)))
	{
		MSG_BOX("CPlayableVera::Add_Children");
		return E_FAIL;
	}

	// 궁 왜곡효과
	CDistortionPointRect::INIT_DESC distortRectDesc;
	distortRectDesc.eCreatLevel = eLevel;
	distortRectDesc.fDuration = 2.f;
	distortRectDesc.fStartDistortionPower = 1.f;
	distortRectDesc.fEndDistortionPower = 0.f;
	distortRectDesc.strMaskTexKey = L"Default_Mask";
	distortRectDesc.strNoiseTexKey = L"Distortion";
	distortRectDesc.vStartUvSpeed = Vector2(0.f, 0.f);
	distortRectDesc.vEndUvSpeed = Vector2(0.f, 0.f);
	distortRectDesc.vStartScale = Vector2(0.1f, 0.1f);
	distortRectDesc.vEndScale = Vector2(30.f, 30.f);
	m_pUltimateDistortEffect = CDistortionPointRect::Create(m_pDevice, m_pContext, distortRectDesc);
	if (FAILED(m_pGameInstance.lock()->Add_Object(eLevel, L"Layer_PlayerEffect", m_pUltimateDistortEffect, &g_TimeScale_Player)))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::CPlayableVera::Bind_ShaderResources()
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

HRESULT Client::CPlayableVera::Set_EventFunctions()
{
	if (FAILED(__super::Set_EventFunctions()))
		return E_FAIL;

	//"EnableBodyCollider"
	//"DisAbleBodyCollider"
	//"EnAbleAttackCollider1"
	//"DisAbleAttackCollider1"
	//"EnAbleAttackCollider2"
	//"DisAbleAttackCollider2"
	//"EnAbleAttackCollider3"
	//"DisAbleAttackCollider3"
	//"ActiveParryFlag"
	//"InActiveParryFlag"
	//"UseSkill1"
	//"UseSkill2"

	EventFunctions["EnableBodyCollider"] = [&]() {
		m_MapColliderCom[L"Col_Body"]->Set_Activate(true); 
		m_MapColliderCom[L"Col_Dodge"]->Set_Activate(false);
		};
	EventFunctions["DisAbleBodyCollider"] = [&]() {
		m_MapColliderCom[L"Col_Body"]->Set_Activate(false); 
		m_MapColliderCom[L"Col_Dodge"]->Set_Activate(true);
		};
	EventFunctions["EnAbleAttackCollider1"] = [&]() { m_MapColliderCom[L"Col_Attack1"]->Set_Activate(true); };
	EventFunctions["DisAbleAttackCollider1"] = [&]() {m_MapColliderCom[L"Col_Attack1"]->Set_Activate(false); };
	EventFunctions["EnAbleAttackCollider2"] = [&]() {m_MapColliderCom[L"Col_Attack2"]->Set_Activate(true); };
	EventFunctions["DisAbleAttackCollider2"] = [&]() {m_MapColliderCom[L"Col_Attack2"]->Set_Activate(false); };
	EventFunctions["EnAbleAttackCollider3"] = [&]() {m_MapColliderCom[L"Col_Attack3"]->Set_Activate(true); };
	EventFunctions["DisAbleAttackCollider3"] = [&]() {m_MapColliderCom[L"Col_Attack3"]->Set_Activate(false); };
	EventFunctions["PlayTrail"] = [&]() { m_pWeaponDown->Play_Trail(); };
	EventFunctions["StopTrail"] = [&]() { m_pWeaponDown->Stop_Trail(); };
	EventFunctions["GroundEffectPlay"] = [&]() { 
		m_pGroundCrack->Get_Transform()->Set_Pos(m_vMyPos);
		m_pGroundCrack->PlayOnce(); 
		};
	EventFunctions["DodgeColEnable"] = [&]() { m_MapColliderCom[L"Col_Dodge"]->Set_Activate(true); };
	EventFunctions["DodgeColDisable"] = [&]() { m_MapColliderCom[L"Col_Dodge"]->Set_Activate(false); };

	return S_OK;
}

void Client::CPlayableVera::OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
{
	if (pCollider->Get_ColliderLayer() == Col_Monster)
	{
		if (pSrcCol == m_MapColliderCom[L"Col_Attack1"] || pSrcCol == m_MapColliderCom[L"Col_Attack2"] || pSrcCol == m_MapColliderCom[L"Col_Attack3"])
		{
			m_pPlayerCamera.lock()->CameraShake(0.05f, XMConvertToRadians(55.f), XMConvertToRadians(60.f));

			static_pointer_cast<CMovableObject>(pCollider->Get_Owner().lock())->Hit(m_iAttackPower);
			if (m_iCurrentState == MeleeAttack)
			{
				m_iSkillBallStack++;
				if (m_iSkillBallStack == 3)
				{
					m_pSkillBallContainer->CreateSkillBall();
					m_iSkillBallStack = 0;
				}

				m_iCurrentSp += 3;
				m_iCurrentSp = min(100, m_iCurrentSp);
				m_pSpSlider->SetSlideRate(Get_SpRate());
			}
		}
	}

	if (m_bCanSuperDodge)
	{
		if (pCollider->Get_ColliderLayer() == Col_MonsterAttack)
		{
			if (pSrcCol == m_MapColliderCom[L"Col_Dodge"])
			{
				m_bCanSuperDodge = false;
				m_bOnSuperDodgeTime = true;
				g_TimeScale_Monster = 0.1f;
				m_StopWatchs[SW_DODGE].StartCheck();
				m_StopWatchs[SW_TIMESTOP].StartCheck();

				Vector3 vPos = m_vMyPos + m_vMyLook * 1.5f;
				vPos.y += 1.f;
				m_pChosanEffect->Get_Transform()->Set_Pos(vPos);
				m_pChosanEffect->PlayOnce();

				m_pGameInstance.lock()->DarkWindowActivate(3.f);
				PlaySoundOnce(L"Snap", 1.f);
				PlaySoundOnce(L"Chsan", 1.f);
			}
		}
	}	
}

void Client::CPlayableVera::OnCollisionStay(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
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
			m_pTransformCom->Set_Pos(vCurPos - vDir * (radiusAdd - fDist), m_pNavigationCom);
		}
	}
}

void Client::CPlayableVera::Move()
{
	if (moveVelocity.Length() > 0.1f)
	{


		if (m_iCurrentState < RunStart || m_iCurrentState == RunStartEnd || m_iCurrentState == Stop)
		{
			Change_State(RunStart);
			moveVelocity = Vector3(0.f, 0.f, 0.f);
		}
		else if (m_iCurrentState == Dodge)
		{		
			if (m_dCurrentAnimFrame < 0.29)
				moveVelocity = Vector3(0.f, 0.f, 0.f);
			else if (m_dCurrentAnimFrame > 0.3)
				Change_State(Run);
		}
		else if (m_iCurrentState >= CorePassive &&
				 m_iCurrentState < PlayableStateEnd)
		{
			if (m_dCurrentAnimFrame < 0.3)
				moveVelocity = Vector3(0.f, 0.f, 0.f);
			else
				Change_State(RunStart);
		}

		Vector3 vDir;

		vDir = AdjustVelocityWithCamera();
		
		Vector4 At = XMVectorSetW(m_pTransformCom->Get_Pos() + vDir, 1.f);
		m_pTransformCom->LookAt(At);
	}
	else
	{		
		if (nullptr != m_pTarget.lock() &&
			m_iCurrentState >= CorePassive &&
			m_iCurrentState <= SkillRedEX &&
			m_dCurrentAnimFrame < 0.1)
		{
			Vector3 vDir = AdjustVelocityWithTarget();
			Vector4 At = XMVectorSetW(m_pTransformCom->Get_Pos() + vDir, 1.f);
			m_pTransformCom->LookAt(At);
		}		
	}

}

Vector3 Client::CPlayableVera::AdjustVelocityWithCamera()
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

Vector3 Client::CPlayableVera::AdjustVelocityWithTarget()
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

void Client::CPlayableVera::DetectNearestTarget()
{
	shared_ptr<CLayer> layer = m_pGameInstance.lock()->Get_Layer(L"Layer_Monster");
	if (layer == nullptr)
		return;

	_float fNearestDist = 20.f;
	Vector3 vPos = m_pTransformCom->Get_Pos();
	shared_ptr<CGameObject> tempTarget = nullptr;
	for (auto& GameObject : layer->Get_ObjList())
	{
		if (static_pointer_cast<CMovableObject>(GameObject)->isDead())
			continue;

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

void Client::CPlayableVera::ComputeTarget()
{
	if (nullptr != m_pTarget.lock())
	{
		if (m_pTarget.lock()->isDead())
		{
			m_pTarget.reset();
			return;
		}

		_float fTargetDist = CMathUtils::GetDistance(m_pTransformCom->Get_Pos(), m_pTarget.lock()->Get_Transform()->Get_Pos());
		if (fTargetDist >= 20.f)
			m_pTarget.reset();
	}
	else
	{
		DetectNearestTarget();
	}
}

void Client::CPlayableVera::Hit(_uint iDamage)
{
	if (m_iCurrentState < CorePassive || m_iCurrentState > SkillRedEX)
		Change_State(hit);

	__super::Hit(iDamage);

	m_iCurrentHp -= iDamage;
	m_iCurrentHp = max(0, m_iCurrentHp);
	m_pHpSlider->SetSlideRate(Get_HpRate());

	m_MapColliderCom[L"Col_Attack1"]->Set_Activate(false);
	m_MapColliderCom[L"Col_Attack2"]->Set_Activate(false);
	m_MapColliderCom[L"Col_Attack3"]->Set_Activate(false);
	m_MapColliderCom[L"Col_Dodge"]->Set_Activate(false);
}

void Client::CPlayableVera::HardHit(_uint iDamage)
{
}

void Client::CPlayableVera::AddCorePassiveStack()
{
	// 3체인후 평타를 쳤으면 3체인트리거를 비워줌
	m_eLastTripleSkillball = SKILL_NONE;

	m_pEneryUIs[m_iCorePassiveStack]->Activate();

	// 1 ~ 에너시UI개수
	m_iCorePassiveStack = min(sizeof(m_pEneryUIs)/sizeof(m_pEneryUIs[0]), m_iCorePassiveStack + 1);
}

void Client::CPlayableVera::UseCorePassive()
{
	m_iCorePassiveStack = 0;

	for (auto& pEnergyUI : m_pEneryUIs)
		pEnergyUI->InActivate();
}

shared_ptr<CPlayableVera> Client::CPlayableVera::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc)
{
	shared_ptr<CPlayableVera> pInstance = make_shared<CPlayableVera>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(initDesc)))
	{
		MSG_BOX("Failed to Created : CPlayableVera");
		pInstance.reset();
	}

	return pInstance;
}