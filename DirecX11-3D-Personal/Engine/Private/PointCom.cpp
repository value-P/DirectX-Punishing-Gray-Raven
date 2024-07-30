#include "PointCom.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "PointProp.h"

Engine::CPointCom::CPointCom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CPrimitiveComponent(pDevice, pContext)
{
}

HRESULT Engine::CPointCom::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    m_pBuffer = m_pPointProperty = CGameInstance::GetInstance()->Get_PointProp();
    if (m_pBuffer == nullptr)
        return E_FAIL;

    return S_OK;
}

HRESULT Engine::CPointCom::Setting_Texture(SpriteTexType eTexType, shared_ptr<CTexture> pTexture)
{
    if (nullptr == pTexture)
        return E_FAIL;

    m_pTextures[(_uint)eTexType] = pTexture;

    return S_OK;
}

HRESULT Engine::CPointCom::Bind_SRV(const _char* pConstantName, SpriteTexType eTexType, _uint iTextureIndex)
{
    if (nullptr == m_pShader || nullptr == m_pTextures[(_uint)eTexType]) return E_FAIL;

    ComPtr<ID3D11ShaderResourceView> pSRV = m_pTextures[(_uint)eTexType]->GetSRV(iTextureIndex);

    if (nullptr == pSRV) return E_FAIL;

    return m_pShader->Bind_SRV(pConstantName, pSRV);
}

HRESULT Engine::CPointCom::Bind_SRVs(const _char* pConstantName, SpriteTexType eTexType)
{
    if (nullptr == m_pShader || nullptr == m_pTextures[(_uint)eTexType]) return E_FAIL;

    if (m_pTextures[(_uint)eTexType]->GetSRVs().size() <= 0) return E_FAIL;

    return m_pShader->Bind_SRVs(pConstantName, m_pTextures[(_uint)eTexType]->GetSRVs());
}

shared_ptr<CPointCom> Engine::CPointCom::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CPointCom> pInstance = make_shared<CPointCom>(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        pInstance.reset();
        MSG_BOX("Failed to Created : CPointCom");
    }

    return pInstance;
}
