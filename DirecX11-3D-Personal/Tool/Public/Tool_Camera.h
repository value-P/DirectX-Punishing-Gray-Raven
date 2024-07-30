#pragma once
#include "Camera.h"

namespace Tool
{
    class CTool_Camera final : public CCamera
    {
	public:
		CTool_Camera();
		virtual ~CTool_Camera() = default;

	public:
		virtual HRESULT Initialize(_float fFov, _float fNear, _float fFar, _float fAspect, _float fMouseSensor, Vector3 vInitPos);
		virtual void Priority_Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta) override;
		virtual void Late_Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	private:
		void Limit_MouseMove();

	private:
		_bool m_bFixCursor = false;

		_float m_fMouseSensor = 0.f;
		//_float m_fCamSpeed = 1.f;
		_float m_fCamSpeed = 25.f;

	public:
		static shared_ptr<CTool_Camera> Create(_float fFov, _float fNear, _float fFar, _float fAspect, Vector3 vInitPos, _float fMouseSensor = 0.1f);
    };
}