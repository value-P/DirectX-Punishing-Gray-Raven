#pragma once
#include "GameObject.h"

namespace Client
{
	class CEffectObject abstract : public CGameObject
	{
	public:
		struct INIT_DESC
		{
			Vector3 vInitPos;
			_float fSpeed;
			LEVEL eCreateLevel;
			_int iDamage;
			_float fDuration;
		};

	public:
		CEffectObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CEffectObject() = default;

	public:
		_bool isActivate() { return m_pCollider->IsActivate(); }

	public:
		virtual HRESULT Initialize(const INIT_DESC& initDesc);
		virtual void Priority_Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta) override;
		virtual void Late_Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	public:
		virtual void Activate() { m_pCollider->Set_Activate(true); m_fDurationCheck = m_fDuration; }
		virtual void InActivate() { m_pCollider->Set_Activate(false);}

	protected:
		virtual HRESULT Add_Component(LEVEL eCreateLevel);

	protected:
		shared_ptr<CCollider> m_pCollider = nullptr;

		Matrix m_matWorld = {};

		Vector3 m_vOriginPos = {};
		_float m_fDurationCheck = 0.f;
		_float m_fDuration = 0.f;
		_float m_fSpeed = 0.f;
		_int m_iDamage = 0;
	};
}