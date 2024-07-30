#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CGameInstance;
	class CUIBase;

	class CUIManager final
	{
	public:
		CUIManager(_uint iNumLevels);
		virtual ~CUIManager() = default;

	public:
		HRESULT Initialize(HWND clientHandle);
		void Tick(_float fTimeDelta);

	public:
		HRESULT Add_UI(_uint iLevelIndex, shared_ptr<CUIBase> pUI);
		void Clear(_uint iLevelIndex);
		void EnableUIs() { m_bActiveUI = true; }
		void DisableUIs() { m_bActiveUI = false; }

	private:
		_uint	m_iLevelNum = 0;
		weak_ptr<CGameInstance> m_pGameInstance;

		vector<list<shared_ptr<CUIBase>>> m_vecUIList;
		list<shared_ptr<CUIBase>> m_clickedUIList;

		HWND m_ClientHandle = {};

		_bool m_bActiveUI = true;

	public:
		static shared_ptr<CUIManager> Create(_uint iNumLevels, HWND clientHandle);
	};
}


