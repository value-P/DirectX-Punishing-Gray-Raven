#include "TerrainCom.h"
#include "TerrainProp.h"
#include "Shader.h"
#include "Texture.h"

Engine::CTerrainCom::CTerrainCom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CPrimitiveComponent(pDevice, pContext)
{
}

HRESULT Engine::CTerrainCom::Initialize(shared_ptr<CTerrainProp> pBufferProperty)
{
    if (FAILED(__super::Initialize())) return E_FAIL;

    if (nullptr == pBufferProperty) return E_FAIL;

    m_pBuffer = m_pTerrainProperty = pBufferProperty;

    return S_OK;
}

HRESULT Engine::CTerrainCom::Setting_Texture(TerrainTexType eTexType, shared_ptr<CTexture> pTexture)
{
    if (nullptr == pTexture)
        return E_FAIL;

    m_pTextures[(_uint)eTexType] = pTexture;

    return S_OK;
}

HRESULT Engine::CTerrainCom::Bind_SRV(const _char* pConstantName, TerrainTexType eTexType, _uint iTextureIndex)
{
    if (nullptr == m_pShader || nullptr == m_pTextures[(_uint)eTexType]) return E_FAIL;

    ComPtr<ID3D11ShaderResourceView> pSRV = m_pTextures[(_uint)eTexType]->GetSRV(iTextureIndex);

    if (nullptr == pSRV) 
        return E_FAIL;

    return m_pShader->Bind_SRV(pConstantName, pSRV);
}

HRESULT Engine::CTerrainCom::Bind_SRVs(const _char* pConstantName, TerrainTexType eTexType)
{
    if (nullptr == m_pShader || nullptr == m_pTextures[(_uint)eTexType]) return E_FAIL;

    if (m_pTextures[(_uint)eTexType]->GetSRVs().size() <= 0) return E_FAIL;

    return m_pShader->Bind_SRVs(pConstantName, m_pTextures[(_uint)eTexType]->GetSRVs());
}

shared_ptr<CTerrainCom> Engine::CTerrainCom::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<CTerrainProp> pBufferProperty)
{
    shared_ptr<CTerrainCom> pInstance = make_shared<CTerrainCom>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pBufferProperty)))
    {
        pInstance.reset();
        MSG_BOX("Failed to Created : CTerrainCom");
    }

    return pInstance;
}