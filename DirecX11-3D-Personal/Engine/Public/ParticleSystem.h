#pragma once
#include "PrimitiveComponent.h"
#include "PointParticle.h"

namespace Engine
{
    class CShader;

    class ENGINE_DLL CParticleSystem : public CPrimitiveComponent
    {
    public:
        enum PARTICLE_OPTION {Drop, Spread, OptEnd};

    public:
        CParticleSystem(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CParticleSystem() = default;

    public:
        void PlayOnce();
        void PlayLoop();
        void Stop();

        _bool isStopped() { return m_bStopped; }
        Vector3 Get_PivotPos() { return m_pParticleProperty->Get_PivotPos(); }
        PARTICLE_OPTION Get_ParticleOption() { return m_eCurrentOption; }

    public:
        HRESULT Initialize(_uint iNumInstance, const CPointParticle::INSTANCE_DESC& initData);
        void Tick(_float fTimeDelta);
        HRESULT Render(_uint iPassIndex, _uint iTechniqueIndex = 0);

        HRESULT Setting_Texture(SpriteTexType eTexType, shared_ptr<CTexture> pTexture);

    public:
        HRESULT Bind_SRV(const _char* pConstantName, SpriteTexType eTexType, _uint iTextureIndex);
        HRESULT Bind_SRVs(const _char* pConstantName, SpriteTexType eTexType);

    public:
        void Change_Option(PARTICLE_OPTION eOption);
        void Change_ParticleData(const CPointParticle::INSTANCE_DESC& particleData);

    private: // 해당 컴포넌트에서만 할 수 있는 기능에 사용하기 위해서
        shared_ptr<CPointParticle> m_pParticleProperty = nullptr;
        shared_ptr<CTexture> m_pTextures[(_uint)SpriteTexType::TEX_END];

    private:
        PARTICLE_OPTION m_eCurrentOption = Drop;
        _bool m_bPlayOnce = false;
        _bool m_bStopped = true;

    public:
        static shared_ptr<CParticleSystem> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumInstance, const CPointParticle::INSTANCE_DESC& initData);

    };
}


