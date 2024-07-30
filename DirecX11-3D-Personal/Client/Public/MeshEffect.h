#pragma once
#include "FX.h"

namespace Client
{
	class CMeshEffect : public CFX
	{
	public:
		struct INIT_DESC
		{
			LEVEL eLevel;
			wstring FXPrefabPath;
			Vector3 vInitPos;
		};

	public:
		CMeshEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CMeshEffect() = default;

	public:
		virtual HRESULT Initialize(const INIT_DESC& InitDesc);
		virtual void Priority_Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta, const Matrix& socket);
		virtual void Late_Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;
		virtual HRESULT Render_Distortion() override;
		virtual HRESULT Render_ExceptDark() override;

	public:
		virtual void PlayOnce() override;
		virtual void PlayLoop() override;
		virtual void Stop() override;
	
	protected:
		HRESULT Add_Component(LEVEL eLevel, const wstring& FXPrefabPath);
		HRESULT Bind_ShaderResources();
		HRESULT Bind_DistortionResurces();
	protected:
		shared_ptr<CEffectMeshCom> m_pEffectCom = nullptr;
		shared_ptr<CTexture> m_pMaskTex = nullptr;
		shared_ptr<CTexture> m_pNoiseTex = nullptr;

	protected:
		_bool m_bUseLoop = false;
		_float m_fDuration = 0.f;
		_float m_fDurationCheck = 0.f;
		_bool m_lastFrameCalled = false;
		_bool m_bUsePrimeColor = false;
		Vector4 m_vPrimeColor = {};
		Matrix m_SocketMatrix = XMMatrixIdentity();
		_double m_fFrameRate = 0.0;

	public:
		static shared_ptr<CMeshEffect> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc);
	};
}


