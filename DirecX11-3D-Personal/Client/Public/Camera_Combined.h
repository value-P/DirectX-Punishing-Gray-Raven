#pragma once
#include "Camera.h"

namespace Client
{
	class CMovableObject;
	class CPlayableVera;

    class CCamera_Combined final: public CCamera
    {
	public:
		enum TARGET_TYPE{COMBINED_OBJECT, TARGET_OBJECT};

	public:
		CCamera_Combined();
		virtual ~CCamera_Combined() = default;

	public:
		virtual HRESULT Initialize(_float fFov, _float fNear, _float fFar, _float fAspect, shared_ptr<CPlayableVera> pCombinedObj, const Vector3& vInitialOffset);
		virtual void Priority_Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta) override;
		virtual void Late_Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	public:
		void LockOn(_bool lockOn);

	private:
		void Compute_SphericalToOrthogonal();
		void Compute_TrackingOwner(_float fTimeDelta);
		void KeyInput(_float fTImeDelta);

	private:
		weak_ptr<CPlayableVera> m_pCombinedObj;
		shared_ptr<CMovableObject> m_pTarget;
		
		_bool m_bLockOn = false;

		Vector3 m_vOffset = {};
		Vector3 m_vCamReference = {};
		Vector3 m_vPrevAt = {};

		_float m_fMinDist = 3.f;
		_float m_fMaxDist = 10.f;

		_float m_fOriginDist = 0.f;
		_float m_fCombinedDist = 0.f;
		_float m_fAzimuthRad = 0.f;		// 방위각 : 좌우 각도
		_float m_fElevationRad = 0.f;	// 양각 :  올라가는 각도

		Vector3 m_vNextEyePos = {};

	public:
		static shared_ptr<CCamera_Combined> Create(_float fFov, _float fNear, _float fFar, _float fAspect, shared_ptr<CPlayableVera> pCombinedObj, const Vector3& vInitialOffset);

    };
}


