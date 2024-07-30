#include "pch.h"
#include "Light_Manager.h"
#include "Light.h"
#include "Shader.h"
#include "RectProp.h"
#include "GameInstance.h"

Engine::CLight_Manager::CLight_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice), m_pContext(pContext)
{
}

const shared_ptr<LIGHT_DESC> Engine::CLight_Manager::Get_LightDesc(_uint iIndex)
{
    if (iIndex >= m_pLightList.size()) return nullptr;

    auto iter = m_pLightList.begin();

    for (_uint i = 0; i < iIndex; ++i)
        ++iter;
     
    return (*iter)->Get_LightDesc();
}

const Matrix* Engine::CLight_Manager::Get_LightViewMatrix(_uint iIndex)
{
    if (iIndex >= m_pLightList.size()) return nullptr;

    auto iter = m_pLightList.begin();

    for (_uint i = 0; i < iIndex; ++i)
        ++iter;

    return (*iter)->Get_ViewMatrix();
}

HRESULT Engine::CLight_Manager::Initialize()
{
    return S_OK;
}

HRESULT Engine::CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{
    shared_ptr<CLight> pLight = CLight::Create(m_pDevice, m_pContext, LightDesc);
    if (nullptr == pLight) return E_FAIL;

    m_pLightList.push_back(pLight);

    return S_OK;
}

HRESULT Engine::CLight_Manager::Render_Light(shared_ptr<CShader> pShader)
{
    shared_ptr<CRectProp> pRectProp = CGameInstance::GetInstance()->Get_RectProp();

    for (auto& pLight : m_pLightList)
        pLight->Render(pShader, pRectProp);

    return E_NOTIMPL;
}

shared_ptr<CLight_Manager> Engine::CLight_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CLight_Manager> pInstance = make_shared<CLight_Manager>(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLight_Manager");
        pInstance.reset();
    }

    return pInstance;
}
