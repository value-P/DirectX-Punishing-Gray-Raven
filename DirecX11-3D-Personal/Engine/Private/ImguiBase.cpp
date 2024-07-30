#include "ImguiBase.h"
#include "GameInstance.h"

Engine::CImguiBase::CImguiBase(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT Engine::CImguiBase::Initialize()
{
    return S_OK;
}

void Engine::CImguiBase::Tick()
{

}

void Engine::CImguiBase::Render()
{
}
