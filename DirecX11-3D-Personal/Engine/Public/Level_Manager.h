#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CLevel;
	class CGameInstance;

	class CLevel_Manager
	{
	public:
		CLevel_Manager();
		virtual ~CLevel_Manager() = default;

	public:
		HRESULT Initialize();
		void Tick(_float fTimeDelta);
		HRESULT Render();

		HRESULT Open_Level(_uint iLevelIndex, shared_ptr<CLevel> pNewLevel);

	private:
		weak_ptr<CGameInstance> m_pGameInstance;
		shared_ptr<CLevel>		m_pCurrentLevel = nullptr;
		_uint					m_iLevelIndex = 0;

	public:
		static shared_ptr<CLevel_Manager> Create();
	};
}
