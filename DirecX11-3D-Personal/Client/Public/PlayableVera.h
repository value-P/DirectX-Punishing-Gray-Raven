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

			Dodge, // ȸ��
			
			CorePassive, 
			MeleeAttack, // �Ϲݰ���, 3ü���� ��Ÿ�� Ư����Ÿ

			SkillBlue, SkillYellow, SkillRed, SkillUltimate, // ��ų
			SkillBlueEX, SkillYellowEX, SkillRedEX,

			hit, OverHit, Stun, BeHitFly, FallDown, // ó������
			EnterRoom, // ������
			Win,// �¸����

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
		// moveVelocity�� ���⼺�� �÷��̾� ĳ���Ͱ� �ƴ� ī�޶� �������� ��ȯ�Ͽ� �� ���⺤�͸��� ��ȯ���ִ� �Լ�
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
		// ���� ������Ʈ
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

		// �÷��̾�� ���� ī�޶�
		weak_ptr<CCamera> m_pPlayerCamera;

		// ũ�� ����Ʈ
		shared_ptr<CMeshEffect> m_pGroundCrack = nullptr;
		shared_ptr<CChosanEffect> m_pChosanEffect = nullptr;

		// �� ����� ����Ʈ
		shared_ptr<CDistortionPointRect> m_pUltimateDistortEffect = nullptr;

	protected:
		// �÷��̾� ���� ���� ����
		_int m_iMeleeStack = 0;
		SkillType m_eLastTripleSkillball = SkillType::SKILL_NONE;
		_int m_iSkillBallStack = 0;
		_int m_iCorePassiveStack = 0;

		// �ʻ�ȸ�� üũ�� ����
		_float m_fSuperDodgeCool = 4.f;
		_float m_fSuperDodgeTime = 3.f;
		_bool m_bCanSuperDodge = true;
		_bool m_bOnSuperDodgeTime = false;

		// �÷��̾� ����
		_int m_iCurrentSp = 100;
		_int m_iMaxSp = 100;

		// ���� ���� �ɼ�
		_bool m_bKillRender = false;

		queue<SkillBallChain> m_SkillInputQueue;

		// ���� ����
		LEVEL m_eCurrentLevel = LEVEL_END;

	public:
		static shared_ptr<CPlayableVera> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc);
	};
}