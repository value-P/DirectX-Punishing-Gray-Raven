#pragma once
#include "FX.h"

namespace Client
{
    class CParticleEffect : public CFX
    {
	public:
		struct INIT_DESC
		{
			LEVEL eLevel;
			wstring FXPrefabPath;
			Vector3 vInitPos;
		};

	public:
		CParticleEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CParticleEffect() = default;

	public:
		virtual HRESULT Initialize(const INIT_DESC& InitDesc);
		virtual void Priority_Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta, const Matrix& socket);
		virtual void Late_Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;
		virtual HRESULT Render_ExceptDark() override;
	public:
		virtual void PlayOnce() override;
		virtual void PlayLoop() override;
		virtual void Stop() override;

	protected:
		HRESULT Add_Component(LEVEL eLevel, const wstring& FXPrefabPath);
		HRESULT Bind_ShaderResources();

	protected:
		shared_ptr<CParticleSystem> m_pParticleSystem = nullptr;
		Matrix m_SocketMatrix = XMMatrixIdentity();

	public:
		static shared_ptr<CParticleEffect> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc);

    };
}


