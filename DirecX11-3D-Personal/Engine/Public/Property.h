#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CGameInstance;

	class ENGINE_DLL CProperty abstract : public enable_shared_from_this<CProperty>
	{
	public:
		CProperty(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CProperty() = default;

	public:
		virtual HRESULT Initialize() { return S_OK; }

	protected:
		ComPtr<ID3D11Device> m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
		weak_ptr<CGameInstance> m_pGameInstance;
	};
}


