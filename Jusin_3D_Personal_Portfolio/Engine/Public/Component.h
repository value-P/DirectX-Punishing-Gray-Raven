#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CGameInstance;

	class ENGINE_DLL CComponent abstract
	{
	public:
		CComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CComponent() = default;

	public:
		virtual HRESULT Initialize();
		virtual HRESULT Render() { return S_OK; }

	protected:
		ComPtr<ID3D11Device> m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
		weak_ptr<CGameInstance> m_pGameInstance;
	};
}
