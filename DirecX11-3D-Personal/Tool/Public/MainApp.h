#pragma once
#include "Engine_Defines.h"

namespace Tool
{
	class CSampleUI;

	class CMainApp final
	{
	public:
		CMainApp();
		virtual ~CMainApp();

	public:
		HRESULT Initialize();
		void	Tick(_float fTimeDelta);
		HRESULT Render();

	private:
		HRESULT Open_Level(LEVEL eStartLevel);
		
	private:
		ComPtr<ID3D11Device>		m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
		weak_ptr<CGameInstance> m_pGameInstance;
		shared_ptr<CSampleUI> TestUI = nullptr;

	public:
		static shared_ptr<CMainApp> Create();
	
	private:
		void Release();
	};
}
