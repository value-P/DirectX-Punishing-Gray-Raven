#include "ParticleSystem.h"
#include "PointParticle.h"
#include "Texture.h"
#include "Shader.h"

Engine::CParticleSystem::CParticleSystem(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CPrimitiveComponent(pDevice, pContext)
{
}

void Engine::CParticleSystem::PlayOnce()
{
    m_bPlayOnce = true;
    m_pParticleProperty->Start();
    m_bStopped = false;
}

void Engine::CParticleSystem::PlayLoop()
{
    m_bPlayOnce = false;
    m_pParticleProperty->Start();
    m_bStopped = false;
}

void Engine::CParticleSystem::Stop()
{
    m_bStopped = true;
}

HRESULT Engine::CParticleSystem::Initialize(_uint iNumInstance, const CPointParticle::INSTANCE_DESC& initData)
{
    m_pBuffer = m_pParticleProperty = CPointParticle::Create(m_pDevice, m_pContext, iNumInstance, initData);

    return S_OK;
}

void Engine::CParticleSystem::Tick(_float fTimeDelta)
{
    if (m_bPlayOnce && m_pParticleProperty->isFinished())
        m_bStopped = true;

    switch (m_eCurrentOption)
    {
    case Engine::CParticleSystem::Drop:
        m_pParticleProperty->Tick_Drop(fTimeDelta);
        break;
    case Engine::CParticleSystem::Spread:
        m_pParticleProperty->Tick_Spread(fTimeDelta);
        break;
    }
}

HRESULT Engine::CParticleSystem::Render(_uint iPassIndex, _uint iTechniqueIndex)
{
    m_pShader->Begin(iPassIndex, iTechniqueIndex);

    m_pParticleProperty->Bind_Buffers();

    m_pParticleProperty->Render();

    return S_OK;
}

HRESULT Engine::CParticleSystem::Setting_Texture(SpriteTexType eTexType, shared_ptr<CTexture> pTexture)
{
    if (nullptr == pTexture)
        return E_FAIL;

    m_pTextures[(_uint)eTexType] = pTexture;

    return S_OK;
}

HRESULT Engine::CParticleSystem::Bind_SRV(const _char* pConstantName, SpriteTexType eTexType, _uint iTextureIndex)
{
    if (nullptr == m_pShader || nullptr == m_pTextures[(_uint)eTexType]) return E_FAIL;

    ComPtr<ID3D11ShaderResourceView> pSRV = m_pTextures[(_uint)eTexType]->GetSRV(iTextureIndex);

    if (nullptr == pSRV) return E_FAIL;

    return m_pShader->Bind_SRV(pConstantName, pSRV);
}

HRESULT Engine::CParticleSystem::Bind_SRVs(const _char* pConstantName, SpriteTexType eTexType)
{
    if (nullptr == m_pShader || nullptr == m_pTextures[(_uint)eTexType]) return E_FAIL;

    if (m_pTextures[(_uint)eTexType]->GetSRVs().size() <= 0) return E_FAIL;

    return m_pShader->Bind_SRVs(pConstantName, m_pTextures[(_uint)eTexType]->GetSRVs());
}

void Engine::CParticleSystem::Change_Option(PARTICLE_OPTION eOption)
{
    m_eCurrentOption = eOption;
}

void Engine::CParticleSystem::Change_ParticleData(const CPointParticle::INSTANCE_DESC& particleData)
{
    m_pParticleProperty->Set_InstanceData(particleData);
}

shared_ptr<CParticleSystem> Engine::CParticleSystem::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumInstance, const CPointParticle::INSTANCE_DESC& initData)
{
    shared_ptr<CParticleSystem> pInstance = make_shared<CParticleSystem>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(iNumInstance, initData)))
    {
        MSG_BOX("Failed to Created : CParticleSystem");
        pInstance.reset();
    }

    return pInstance;
}
