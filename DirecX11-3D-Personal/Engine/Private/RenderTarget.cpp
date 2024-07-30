#include "RenderTarget.h"
#include "Shader.h"
#include "RectProp.h"

Engine::CRenderTarget::CRenderTarget(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice(pDevice), m_pContext(pContext)
{
}

HRESULT Engine::CRenderTarget::Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const Vector4& vClearColor)
{
    m_vClearColor = vClearColor;

    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Width = iSizeX;
    textureDesc.Height = iSizeY;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = ePixelFormat;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&textureDesc, nullptr, m_pTexture2D.GetAddressOf())))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D.Get(), nullptr, m_pRTV.GetAddressOf())))
        return E_FAIL;

    if(FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D.Get(), nullptr, m_pSRV.GetAddressOf())))
        return E_FAIL;

    return S_OK;
}

HRESULT Engine::CRenderTarget::Clear()
{
    m_pContext->ClearRenderTargetView(m_pRTV.Get(), reinterpret_cast<_float*>(&m_vClearColor));

    return S_OK;
}

HRESULT Engine::CRenderTarget::Bind_SRV(shared_ptr<CShader> pShader, const _char* pConstantName)
{
    return pShader->Bind_SRV(pConstantName, m_pSRV);
}

#ifdef _DEBUG
HRESULT Engine::CRenderTarget::Init_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    D3D11_VIEWPORT viewPortDesc = {};
    _uint iNumViewPorts = 1;

    m_pContext->RSGetViewports(&iNumViewPorts, &viewPortDesc);

    m_WorldMatrix = XMMatrixIdentity();

    m_WorldMatrix._11 = fSizeX;
    m_WorldMatrix._22 = fSizeY;
    m_WorldMatrix._41 = fX - viewPortDesc.Width * 0.5f;
    m_WorldMatrix._42 = -fY + viewPortDesc.Height * 0.5f;

    return S_OK;
}

HRESULT Engine::CRenderTarget::Render(shared_ptr<CShader> pShader, shared_ptr<CRectProp> pRectBuffer)
{
    if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(pShader->Bind_SRV("g_Texture", m_pSRV)))
        return E_FAIL;

    pShader->Begin(0, 0);

    pRectBuffer->Bind_Buffers();

    pRectBuffer->Render();

    return S_OK;
}
#endif

shared_ptr<CRenderTarget> Engine::CRenderTarget::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const Vector4& vClearColor)
{
    shared_ptr<CRenderTarget> pInstance = make_shared<CRenderTarget>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(iSizeX, iSizeY, ePixelFormat, vClearColor)))
    {
        MSG_BOX("Failed to Created : CRenderTarget");
        pInstance.reset();
    }

    return pInstance;
}
