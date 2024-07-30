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

			CorePassiveStart, CorePassiveGauging, CorePassiveParring, CorePassiveAttack, // 코패: 기모으기, 반격 , 돌진공격 
			
			Dodge, // 회피
			MeleeAttack, SpecialMelee, // 일반공격, 3체인후 평타시 특수평타

			SkillBlue, SkillYellow, SkillRed, SkillUltimate, // 스킬
			SkillYellowEX, SkillRedEX,

			hit, OverHit, Stun, BeHitFly, FallDown, // 처맞을때
			EnterRoom, // 입장모션
			Win,// 승리모션

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
		// moveVelocity의 방향성을 플레이어 캐릭터가 아닌 카메라 기준으로 변환하여 그 방향벡터만을 반환해주는 함수
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
