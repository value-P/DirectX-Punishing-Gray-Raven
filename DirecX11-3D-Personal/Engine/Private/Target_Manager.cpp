#include "Target_Manager.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "RectProp.h"
#include "GameInstance.h"

Engine::CTarget_Manager::CTarget_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice), m_pContext(pContext)
{
}

Engine::CTarget_Manager::~CTarget_Manager()
{
}

HRESULT Engine::CTarget_Manager::Initialize()
{
    return S_OK;
}

HRESULT Engine::CTarget_Manager::Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const Vector4& vClearColor)
{
    if (nullptr != Find_RenderTarget(strTargetTag))
        return E_FAIL;

    shared_ptr<CRenderTarget> pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iSizeX, iSizeY, ePixelFormat, vClearColor);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    m_RenderTargets.emplace(strTargetTag, pRenderTarget);

    return S_OK;
}

HRESULT Engine::CTarget_Manager::Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
    shared_ptr<CRenderTarget> pRenderTarget = Find_RenderTarget(strTargetTag);

    if (nullptr == pRenderTarget)
        return E_FAIL;

    list< shared_ptr<CRenderTarget>>* pMRTList = Find_MRT(strMRTTag);
    if (nullptr == pMRTList)
    {
        list<shared_ptr<CRenderTarget>> MRTList;
        MRTList.push_back(pRenderTarget);

        m_MRTs.emplace(strMRTTag, MRTList);
    }
    else
        pMRTList->push_back(pRenderTarget);

    return S_OK;
}

HRESULT Engine::CTarget_Manager::Begin_MRT(const wstring& strMRTTag, ComPtr<ID3D11DepthStencilView> pDSView)
{
    ID3D11ShaderResourceView* null[D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr};
    m_pContext->PSSetShaderResources(0, D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, null);

    list<shared_ptr<CRenderTarget>>* pMRTList = Find_MRT(strMRTTag);
    ID3D11RenderTargetView* pRTVs[8] = { nullptr };

    if (nullptr == pMRTList)
        return E_FAIL;

    m_pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

    _uint iNumRTVs = 0;

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Clear();
        pRTVs[iNumRTVs++] = pRenderTarget->Get_RTV().Get();
    }

    if(nullptr == pDSView)
        m_pContext->OMSetRenderTargets(iNumRTVs, pRTVs, m_pDepthStencilView);
    else
    {
        m_pContext->ClearDepthStencilView(pDSView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
        m_pContext->OMSetRenderTargets(iNumRTVs, pRTVs, pDSView.Get());
    }

    return S_OK;
}

HRESULT Engine::CTarget_Manager::End_MRT()
{
    ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    m_pContext->PSSetShaderResources(0, 2, null);

    m_pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);

    Safe_Release(m_pBackBufferView);
    Safe_Release(m_pDepthStencilView);

    return S_OK;
}

HRESULT Engine::CTarget_Manager::Bind_SRV(const wstring& strTargetTag, const shared_ptr<CShader> pShader, const _char* pConstantName)
{
    shared_ptr<CRenderTarget> pRenderTarget = Find_RenderTarget(strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    return pRenderTarget->Bind_SRV(pShader, pConstantName);
}

#ifdef _DEBUG
HRESULT Engine::CTarget_Manager::Init_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
    shared_ptr<CRenderTarget> pRenderTarget = Find_RenderTarget(strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    return pRenderTarget->Init_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT Engine::CTarget_Manager::Render_MRT(const wstring& strMRTTag, const shared_ptr<CShader> pShader)
{
    list< shared_ptr<CRenderTarget>>* pMRTList = Find_MRT(strMRTTag);
    if (nullptr == pMRTList)
        return E_FAIL;

    shared_ptr<CRectProp> pRect = CGameInstance::GetInstance()->Get_RectProp();

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Render(pShader, pRect);
    }

    return S_OK;
}
#endif // _DEBUG

shared_ptr<CRenderTarget> Engine::CTarget_Manager::Find_RenderTarget(const wstring& strTargetTag)
{
    auto iter = m_RenderTargets.find(strTargetTag);

    if (iter == m_RenderTargets.end())
        return nullptr;

    return iter->second;
}

list<shared_ptr<CRenderTarget>>* Engine::CTarget_Manager::Find_MRT(const wstring& strMRTTag)
{
    auto iter = m_MRTs.find(strMRTTag);

    if (iter == m_MRTs.end())
        return nullptr;

    return &iter->second;
}

shared_ptr<CTarget_Manager> Engine::CTarget_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CTarget_Manager> pInstance = make_shared<CTarget_Manager>(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CTarget_Manager");
        pInstance.reset();
    }

    return pInstance;
}
