#include "Property.h"
#include "GameInstance.h"

Engine::CProperty::CProperty(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(CGameInstance::GetInstance())
{
}
