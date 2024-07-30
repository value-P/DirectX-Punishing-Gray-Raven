#pragma once
#include "MovableObject.h"

namespace Engine
{
	class CCamera;
}

namespace Client
{
	class CWeapon;
	class CSkillBtnContainer;
	class CSlider;
	class CUltimateSkillBtn;
	class CDefaultUI;
	class CMeshEffect;
	class CChosanEffect;
	class CFontUI;
	class CDistortionPointRect;

	class CPlayableVera : public CMovableObject
	{
	public:
		struct INIT_DESC
		{
			LEVEL ePropLevel;
			LEVEL eCreateLevel;
			wstring strModelKey;
			Vector3 vInitPos;
			_int iHp;
			_int iAttackPower;
		};

	public:
		enum PlayableState
		{
			Idle,Idle2,IdleChange, 

			RunStart, RunStartEnd, Run, RunR, RunL,

			Stop,

			Dodge, // 회피
			
			CorePassive, 
			MeleeAttack, // 일반공격, 3체인후 평타시 특수평타

			SkillBlue, SkillYellow, SkillRed, SkillUltimate, // 스킬
			SkillBlueEX, SkillYellowEX, SkillRedEX,

			hit, OverHit, Stun, BeHitFly, FallDown, // 처맞을때
			EnterRoom, // 입장모션
			Win,// 승리모션

			PlayableStateEnd
		};

		enum SkillType {SKILL_NONE, BLUE, RED, YELLOW, ULTIMATE};

		struct SkillBallChain
		{
			SkillType eSkill;
			_int iChain;
		};

	public:
		CPlayableVera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CPlayableVera() = default;

	public:
		_float Get_SpRate() { return static_cast<_float>(m_iCurrentSp) / static_cast<_float>(m_iMaxSp); }
		weak_ptr<CMovableObject> Get_TargetPtr() { return m_pTarget; }
		void UseSkill(SkillType eSkillColor, _int iChain);
		_bool UseUltimateSkill();
		void Set_CombinedCamera(shared_ptr<CCamera> pCamera) { m_pPlayerCamera = pCamera; }
		void PlayWinMotion();
		void PlayBornSound();
		void Set_Target(shared_ptr<CMovableObject> pTarget) { m_pTarget = pTarget; }
		void Kill_Render() { m_bKillRender = true; }

	public:
		virtual HRESULT Initialize(const INIT_DESC& initDesc);
		virtual void Priority_Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta) override;
		virtual void Late_Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;
		virtual HRESULT Render_Shadow() override;

	protected:
		virtual void KeyInput() override;
		void SkillBallQueueCheck();
		void InputDodge();
		void InputAttack();
		void InputMovement();
		HRESULT Init_UI(const INIT_DESC& initDesc);

		void PlaySound();

	protected:
		HRESULT Add_StateFunction();
		virtual HRESULT Add_Component(const INIT_DESC& initDesc);
		HRESULT Add_Children(LEVEL eLevel);
		HRESULT Bind_ShaderResources();
		virtual HRESULT Set_EventFunctions() override;

		virtual void OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider) override;
		virtual void OnCollisionStay(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider) override;

	protected:
		// moveVelocity의 방향성을 플레이어 캐릭터가 아닌 카메라 기준으로 변환하여 그 방향벡터만을 반환해주는 함수
		void Move();
		Vector3 AdjustVelocityWithCamera();
		Vector3 AdjustVelocityWithTarget();
		void DetectNearestTarget();
		void ComputeTarget();

		virtual void Hit(_uint iDamage) override;
		virtual void HardHit(_uint iDamage) override;

		void AddCorePassiveStack();
		void UseCorePassive();

	protected:
		// 무기 오브젝트
		shared_ptr<CWeapon> m_pWeaponUpper = nullptr;
		shared_ptr<CWeapon> m_pWeaponDown = nullptr;
		
		// UI
		shared_ptr< CSkillBtnContainer> m_pSkillBallContainer = nullptr;
		shared_ptr<CSlider> m_pHpSlider = nullptr;
		shared_ptr<CSlider> m_pSpSlider = nullptr;
		shared_ptr<CUltimateSkillBtn> m_pUltimateBtn = nullptr;
		shared_ptr<CDefaultUI> m_pEneryUIs[3];
		shared_ptr<CDefaultUI> m_pTargetHpBg = nullptr;
		shared_ptr<CSlider> m_pTargetHp = nullptr;
		shared_ptr<CFontUI> m_pTargetNameFont = nullptr;

		// 플레이어에게 붙은 카메라
		weak_ptr<CCamera> m_pPlayerCamera;

		// 크랙 이펙트
		shared_ptr<CMeshEffect> m_pGroundCrack = nullptr;
		shared_ptr<CChosanEffect> m_pChosanEffect = nullptr;

		// 궁 디스토션 이펙트
		shared_ptr<CDistortionPointRect> m_pUltimateDistortEffect = nullptr;

	protected:
		// 플레이어 공격 관련 변수
		_int m_iMeleeStack = 0;
		SkillType m_eLastTripleSkillball = SkillType::SKILL_NONE;
		_int m_iSkillBallStack = 0;
		_int m_iCorePassiveStack = 0;

		// 초산회피 체크용 변수
		_float m_fSuperDodgeCool = 4.f;
		_float m_fSuperDodgeTime = 3.f;
		_bool m_bCanSuperDodge = true;
		_bool m_bOnSuperDodgeTime = false;

		// 플레이어 스탯
		_int m_iCurrentSp = 100;
		_int m_iMaxSp = 100;

		// 렌더 끄기 옵션
		_bool m_bKillRender = false;

		queue<SkillBallChain> m_SkillInputQueue;

		// 현재 레벨
		LEVEL m_eCurrentLevel = LEVEL_END;

	public:
		static shared_ptr<CPlayableVera> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc);
	};
}