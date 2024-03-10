#include "Level.h"

Engine::CLevel::CLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT Engine::CLevel::Initialize()
{
    return S_OK;
}

void Engine::CLevel::Tick(_float fTimeDelta)
{
}

HRESULT Engine::CLevel::Render()
{
    return S_OK;
}
