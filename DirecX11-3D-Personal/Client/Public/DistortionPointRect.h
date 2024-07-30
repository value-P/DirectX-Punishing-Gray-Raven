#pragma once
#include "FX.h"

namespace Client
{
    class CDistortionPointRect final: public CFX
    {
    public:
        struct INIT_DESC
        {
			LEVEL eCreatLevel;
			wstring strMaskTexKey;
			wstring strNoiseTexKey;
			_float	fDuration;

			Vector2 vStartUvSpeed;
			Vector2 vEndUvSpeed;

			Vector2 vStartScale;
			Vector2 vEndScale;

			//_float vStartRotationSpeed;
			//_float vEndRotationSpeed;

			_float	fStartDistortionPower;
			_float	fEndDistortionPower;
        };

	public:
		CDistortionPointRect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CDistortionPointRect() = default;

	public:
		virtual HRESULT Initialize(const INIT_DESC& InitDesc);
		virtual void Priority_Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta, const Matrix& socket);
		virtual void Late_Tick(_float fTimeDelta) override;
		virtual HRESULT Render_Distortion() override;

	public:
		virtual void PlayOnce() override;
		virtual void PlayLoop() override;
		virtual void Stop() override;

	protected:
		HRESULT Add_Component(LEVEL eLevel);
		HRESULT Bind_DistortionResources();

	protected:
		shared_ptr<CPointCom> m_pPointCom = nullptr;

	protected:
		_bool m_bUseLoop = false;
		
		_float m_fDurationCheck = 0.f;
		_float m_fFrameRate = 0.0;
		_float m_fDuration = 0.f;
		Vector2 m_vUvMoveAcc = {};

		wstring m_strMaskTexKey = L"";
		wstring m_strNoiseTexKey = L"";

		// Uv이동
		Vector2 m_vStartUvSpeed = {};
		Vector2 m_vEndUvSpeed= {};
		Vector2 m_vCurrentUvMoveSpeed = {};

		// 스케일
		Vector2 m_vStartScale= {};
		Vector2 m_vEndScale = {};
		Vector2 m_vCurrentScale = {};

		// 회전
		//_float m_vStartRotationSpeed	= 0.f;
		//_float m_vEndRotationSpeed		= 0.f;
		//_float m_vCurrentRoationSpeed	= 0.f;

		// 디스토션파워
		_float	m_fStartDistortionPower = 0.f;
		_float	m_fEndDistortionPower	= 0.f;
		_float  m_fCurrentDistortionPower = 0.f;

		Matrix m_SocketMatrix = XMMatrixIdentity();

	public:
		static shared_ptr<CDistortionPointRect> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc);
	};
}


