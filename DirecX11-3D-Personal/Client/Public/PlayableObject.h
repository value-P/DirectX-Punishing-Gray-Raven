#pragma once
#include "MovableObject.h"

namespace Client
{
	class CWeapon;

	class CPlayableObject : public CMovableObject
	{
	public:
		struct INIT_DESC
		{
			LEVEL ePropLevel;
			LEVEL eCreateLevel;
			wstring strModelKey;
			Vector3 vInitPos;
		};

	public:
		enum PlayableState
		{
			Idle,Idle2,IdleChange, 

			RunStart, RunStartEnd, Run, RunR, RunL,

			Stop,

			CorePassiveStart, CorePassiveGauging, CorePassiveParring, CorePassiveAttack, // ����: �������, �ݰ� , �������� 
			
			Dodge, // ȸ��
			MeleeAttack, SpecialMelee, // �Ϲݰ���, 3ü���� ��Ÿ�� Ư����Ÿ

			SkillBlue, SkillYellow, SkillRed, SkillUltimate, // ��ų
			SkillYellowEX, SkillRedEX,

			hit, OverHit, Stun, BeHitFly, FallDown, // ó������
			EnterRoom, // ������
			Win,// �¸����

			PlayableStateEnd
		};

		enum SkillType {SKILL_NONE, BLUE, RED, YELLOW, ULTIMATE};

	public:
		CPlayableObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CPlayableObject() = default;

	public:
		weak_ptr<CGameObject> Get_TargetPtr() { return m_pTarget; }

	public:
		virtual HRESULT Initialize(const INIT_DESC& initDesc);
		virtual void Priority_Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta) override;
		virtual void Late_Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	protected:
		virtual void KeyInput() override;
		void SkillBallQueueCheck();
		void InputDodge();
		void InputAttack();
		void InputMovement();

	protected:
		HRESULT Add_StateFunction();
		virtual HRESULT Add_Component(const INIT_DESC& initDesc);
		HRESULT Add_Children(LEVEL eLevel);
		HRESULT Bind_ShaderResources();

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

	protected:
		shared_ptr<CWeapon> m_pWeapon;

	protected:
		_bool m_bUltimateMode = false;
		_bool m_bHit = false;

		_bool m_bBlueSkillChained = false;

		_int m_iParringStack = 0;
		_int m_iMeleeStack = 0;

		queue<SkillType> m_SkillInputQueue;

	public:
		static shared_ptr<CPlayableObject> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc);
	};
}
