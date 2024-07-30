#include "Component.h"
#include "GameInstance.h"

Engine::CComponent::CComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice),
	m_pContext(pContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT Engine::CComponent::Initialize()
{
	return S_OK;
}
