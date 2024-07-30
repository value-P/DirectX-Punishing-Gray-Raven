#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CCamera;

	class CCameraManager
	{
	public:
		CCameraManager(_uint iNumLevels);
		virtual ~CCameraManager() = default;

	public:
		HRESULT Initialize();
		void Priority_Tick(_float fTimeDelta);
		void Tick(_float fTimeDelta);
	
	public:
		void Set_CurrentLevel(_uint iLevel) { m_iCurrentLevel = iLevel; }

		HRESULT Add_Camera(_uint iLevelIndex, const wstring& strCameraTag, shared_ptr<CCamera> pCamera);
		void Change_Scene(_uint iLevelIndex);
		HRESULT Set_MainCamera(_uint iLevelIndex, const wstring& strCameraTag);

	private:
		_uint	m_iLevelNum = 0;
		_uint	m_iCurrentLevel = 0;

		typedef map<const wstring, shared_ptr<CCamera>>	CAMERAS;
		vector<CAMERAS> m_vecCameras;
		vector<shared_ptr<CCamera>>m_MainCameras;

	public:
		static shared_ptr<CCameraManager> Create(_uint iNumLevels);
	};
}


