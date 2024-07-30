#include "SpriteCom.h"
#include "RectProp.h"
#include "Shader.h"
#include "Texture.h"

Engine::CSpriteCom::CSpriteCom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CPrimitiveComponent(pDevice, pContext)
{
}

HRESULT Engine::CSpriteCom::Initialize(shared_ptr<CRectProp> pBufferProperty)
{
    if (FAILED(__super::Initialize())) return E_FAIL;

    if (nullptr == pBufferProperty) return E_FAIL;

    m_pBuffer = m_pRectProperty = pBufferProperty;

    return S_OK;
}

HRESULT Engine::CSpriteCom::Setting_Texture(SpriteTexType eTexType, shared_ptr<CTexture> pTexture)
{
    if (nullptr == pTexture)
        return E_FAIL;

    m_pTextures[(_uint)eTexType] = pTexture;

    return S_OK;
}

HRESULT Engine::CSpriteCom::Bind_SRV(const _char* pConstantName, SpriteTexType eTexType, _uint iTextureIndex)
{
    if (nullptr == m_pShader || nullptr == m_pTextures[(_uint)eTexType]) return E_FAIL;

    ComPtr<ID3D11ShaderResourceView> pSRV = m_pTextures[(_uint)eTexType]->GetSRV(iTextureIndex);

    if (nullptr == pSRV) return E_FAIL;

    return m_pShader->Bind_SRV(pConstantName, pSRV);
}

HRESULT Engine::CSpriteCom::Bind_SRVs(const _char* pConstantName, SpriteTexType eTexType)
{
    if (nullptr == m_pShader || nullptr == m_pTextures[(_uint)eTexType]) return E_FAIL;

    if (m_pTextures[(_uint)eTexType]->GetSRVs().size() <= 0) return E_FAIL;

    return m_pShader->Bind_SRVs(pConstantName, m_pTextures[(_uint)eTexType]->GetSRVs());
}

shared_ptr<CSpriteCom> Engine::CSpriteCom::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<CRectProp> pBufferProperty)
{
    shared_ptr<CSpriteCom> pInstance = make_shared<CSpriteCom>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pBufferProperty)))
    {
        pInstance.reset();
        MSG_BOX("Failed to Created : CSpriteCom");
    }

    return pInstance;
}
