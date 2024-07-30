#pragma once
#include "Camera.h"

namespace Client
{
	class CCamera_Free final: public CCamera
	{
	public:
		CCamera_Free();
		CCamera_Free(const CCamera_Free& rhs);
		virtual ~CCamera_Free() = default;

	public:
		virtual HRESULT Initialize(_float fFov, _float fNear, _float fFar, _float fAspect, _float fMouseSensor, Vector3 vInitPos);
		virtual void Priority_Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta) override;
		virtual void Late_Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	private:
		_float m_fMouseSensor = 0.1f;
		_float m_fCamSpeed = 15.f;

	public:
		static shared_ptr<CCamera_Free> Create(_float fFov, _float fNear, _float fFar, _float fAspect, Vector3 vInitPos, _float fMouseSensor = 0.1f);
	};
}


