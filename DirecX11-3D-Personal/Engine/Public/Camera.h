#pragma once
#include "GameObject.h"

namespace Engine
{
    class ENGINE_DLL CCamera abstract: public CGameObject
    {
    public:
        CCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		CCamera(const CCamera& rhs);
        virtual ~CCamera() = default;

	public:
		void CameraShake(_float fDuration, _float fMinFov, _float fMaxFov);

	public:
		virtual HRESULT Initialize(_float fFov, _float fNear, _float fFar, _float fAspect, Vector3 vInitPos);
		virtual void Priority_Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta) override;
		virtual void Late_Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	protected:
		HRESULT SetUp_Matrices();

	protected:
		_float		m_fFov = 0.f;
		_float		m_fOriginFov = 0.f;
		_float		m_fNear = 0.f;
		_float		m_fFar = 0.f;
		_float		m_fAspect = 0.f;

		// 쉐이킹 체크용 함수
		_float		m_fMinShakeFov = 0.f;
		_float		m_fMaxShakeFov = 0.f;
		_bool		m_bOnShake = false;
		_float		m_fShakeDuration = 0.f;
		_float		m_fShakeCheck = 0.f;
	};
}