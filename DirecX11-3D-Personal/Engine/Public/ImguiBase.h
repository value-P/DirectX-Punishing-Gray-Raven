#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CGameInstance;

	class ENGINE_DLL CImguiBase abstract
	{
	public:
		CImguiBase(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CImguiBase() = default;

	public:
		virtual HRESULT Initialize();
		virtual void Tick();
		virtual void Render();
		
	protected:
		ComPtr<ID3D11Device> m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

	protected:
		weak_ptr<CGameInstance> m_pGameInstance;
	};
}


