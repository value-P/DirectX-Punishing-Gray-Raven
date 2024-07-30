#include "NonAnimEffectProp.h"
#include "NonAnimModelProp.h"

Engine::CNonAnimEffectProp::CNonAnimEffectProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CEffectProp(pDevice, pContext)
{
}

HRESULT Engine::CNonAnimEffectProp::Add_Buffer(const wstring& strBufferPath, const Matrix& pivotMatrix)
{
    if (m_pModelProp != nullptr)
        return E_FAIL;

    shared_ptr<CNonAnimModelProp> pModelProp = CNonAnimModelProp::Create(m_pDevice, m_pContext, strBufferPath, pivotMatrix);
    if (nullptr == pModelProp)
        return E_FAIL;

    m_pModelProp = pModelProp;

    return S_OK;
}

HRESULT Engine::CNonAnimEffectProp::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CNonAnimEffectProp> Engine::CNonAnimEffectProp::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CNonAnimEffectProp> pInstance = make_shared<CNonAnimEffectProp>(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CNonAnimEffectProp");
        pInstance.reset();
    }

    return pInstance;
}
