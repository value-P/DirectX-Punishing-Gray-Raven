#include "BoxCom.h"
#include "CubeProp.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"

Engine::CBoxCom::CBoxCom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CPrimitiveComponent(pDevice, pContext)
{
}

HRESULT Engine::CBoxCom::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    m_pBuffer = m_pCubeProperty = CGameInstance::GetInstance()->Get_CubeProp();
    if (m_pBuffer == nullptr)
        return E_FAIL;

    return S_OK;
}

HRESULT Engine::CBoxCom::Setting_Texture(SpriteTexType eTexType, shared_ptr<CTexture> pTexture)
{
    if (nullptr == pTexture)
        return E_FAIL;

    m_pTextures[(_uint)eTexType] = pTexture;

    return S_OK;
}

HRESULT Engine::CBoxCom::Bind_SRV(const _char* pConstantName, SpriteTexType eTexType, _uint iTextureIndex)
{
    if (nullptr == m_pShader || nullptr == m_pTextures[(_uint)eTexType]) return E_FAIL;

    ComPtr<ID3D11ShaderResourceView> pSRV = m_pTextures[(_uint)eTexType]->GetSRV(iTextureIndex);

    if (nullptr == pSRV) return E_FAIL;

    return m_pShader->Bind_SRV(pConstantName, pSRV);
}

HRESULT Engine::CBoxCom::Bind_SRVs(const _char* pConstantName, SpriteTexType eTexType)
{
    if (nullptr == m_pShader || nullptr == m_pTextures[(_uint)eTexType]) return E_FAIL;

    if (m_pTextures[(_uint)eTexType]->GetSRVs().size() <= 0) return E_FAIL;

    return m_pShader->Bind_SRVs(pConstantName, m_pTextures[(_uint)eTexType]->GetSRVs());
}

shared_ptr<CBoxCom> Engine::CBoxCom::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CBoxCom> pInstance = make_shared<CBoxCom>(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        pInstance.reset();
        MSG_BOX("Failed to Created : CBoxCom");
    }

    return pInstance;
}
