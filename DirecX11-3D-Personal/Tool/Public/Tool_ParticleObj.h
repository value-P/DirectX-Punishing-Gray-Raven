#pragma once
#include "GameObject.h"
#include "ParticleSystem.h"

namespace Engine
{
	class CTexture;
}

namespace Tool
{
    class CTool_ParticleObj final : public CGameObject
    {
	public:
		CTool_ParticleObj(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CTool_ParticleObj() = default;

	public:
		void PlayOnce();
		void PlayLoop();
		void Stop();

	public:
		virtual HRESULT Initialize(_uint iNumInstance, const Vector3& vInitPos, LEVEL eLevel, const wstring& strTextureKey, const CPointParticle::INSTANCE_DESC& initData);
		virtual void Priority_Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta) override;
		virtual void Late_Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	public:
		void Change_Option(CParticleSystem::PARTICLE_OPTION eTickOption);
		void Change_InstanceData(const CPointParticle::INSTANCE_DESC& InstanceData);
		HRESULT Set_Texture(LEVEL eLevel, const wstring& strTextureKey);
		HRESULT Set_Texture(shared_ptr<CTexture> pTexture);

	private:
		HRESULT Add_Component(_uint iNumInstance, const CPointParticle::INSTANCE_DESC& initData);
		HRESULT Bind_ShaderResources();

	private:
		shared_ptr<CParticleSystem> m_pParticleSystem = nullptr;

	public:
		static shared_ptr<CTool_ParticleObj> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumInstance, const Vector3& vInitPos, LEVEL eLevel, const wstring& strTextureKey, const CPointParticle::INSTANCE_DESC& initData);

    };
}


