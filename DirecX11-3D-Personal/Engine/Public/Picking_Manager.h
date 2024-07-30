#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CGameInstance;

	class CPicking_Manager
	{
	public:
		CPicking_Manager(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
		virtual ~CPicking_Manager() = default;

	public:
		Ray RayAtViewSpace();
		Ray RayAtWorldSpace();
		Ray RayAtLocalSpace(const Matrix& matWorld);

	private:
		HWND m_hWnd = {};
		_uint m_iWinSizeX = 0;
		_uint m_iWinSizeY = 0;
		weak_ptr<CGameInstance> m_pGameInstance;

	public:
		static shared_ptr<CPicking_Manager> Create(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	};
}


