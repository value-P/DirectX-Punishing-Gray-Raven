#pragma once
#include "FX.h"

namespace Client
{
    class CDistortEffect : public CFX
    {
	public:
		struct INIT_DESC
		{
			LEVEL eLevel;
			wstring FXPrefabPath;
			Vector3 vInitPos;
			wstring strNoiseTexKey;
			_float fDistortionPower;
		};

	public:
		CDistortEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CDistortEffect() = default;

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
		HRESULT Add_Component(LEVEL eLevel, const wstring& FXPrefabPath, const wstring& strNoiseTexKey);
		HRESULT Bind_ShaderResources();

	protected:
		shared_ptr<CEffectMeshCom> m_pEffectCom = nullptr;

	protected:
		_bool m_bUseLoop = false;
		_float m_fDuration = 0.f;
		_float m_fDurationCheck = 0.f;
		_bool m_lastFrameCalled = false;
		shared_ptr<CTexture> m_pNoiseTexture = nullptr;
		Vector4 m_vPrimeColor = {};
		Matrix m_SocketMatrix = XMMatrixIdentity();
		_double m_fFrameRate = 0.0;
		_float m_fDistortionPower = 1.f;
	public:
		static shared_ptr<CDistortEffect> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc);
	};
}


