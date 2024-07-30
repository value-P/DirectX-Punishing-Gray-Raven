#include "TrailCom.h"
#include "TrailBufferPop.h"
#include "Shader.h"
#include "Texture.h"

Engine::CTrailCom::CTrailCom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CPrimitiveComponent(pDevice,pContext)
{
}

HRESULT Engine::CTrailCom::Initialize(_int iMaxTrailRectNum)
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    m_pTrailBuffer = CTrailBufferPop::Create(m_pDevice, m_pContext, iMaxTrailRectNum);

    return S_OK;
}

void Engine::CTrailCom::Tick(const Vector3& vPosUpperWorld, const Vector3& vPosDownWorld)
{
    m_pTrailBuffer->Tick(vPosUpperWorld, vPosDownWorld);
}

HRESULT Engine::CTrailCom::Render(_uint iPassIndex, _uint iTechniqueIndex)
{
    m_pShader->Begin(iPassIndex, iTechniqueIndex);

    m_pTrailBuffer->Bind_Buffers();

    m_pTrailBuffer->Render();

    return S_OK;
}

HRESULT Engine::CTrailCom::Setting_Texture(shared_ptr<CTexture> pTexture)
{
    if (nullptr == pTexture)
        return E_FAIL;

    m_pTexture = pTexture;

    return S_OK;
}

HRESULT Engine::CTrailCom::Bind_SRV(const _char* pConstantName)
{
    if (nullptr == m_pShader || nullptr == m_pTexture)
        return E_FAIL;
    
    return m_pShader->Bind_SRV(pConstantName, m_pTexture->GetSRV(0));
}

shared_ptr<CTrailCom> Engine::CTrailCom::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _int iMaxTrailRectNum)
{
    shared_ptr<CTrailCom> pInstance = make_shared<CTrailCom>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(iMaxTrailRectNum)))
    {
        pInstance.reset();
        MSG_BOX("Failed to Created : CTrailCom");
    }

    return pInstance;
}
