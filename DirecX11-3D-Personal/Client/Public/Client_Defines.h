#pragma once

namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY,LEVEL_BOSS,LEVEL_BOSS2, LEVEL_END };

	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum StopWatchType { SW_Default, SW_0, SW_1 ,SW_2, SW_3, SW_SHIELD, SW_DODGE, SW_TIMESTOP, SW_FOOTSTEP, SW_END};

	struct StopWatch
	{
		void Clear() { Start = false; fTime = 0.f; }
		void StartCheck() { Start = true; }

		bool Start = false;
		float fTime = 0.f;
	};

	enum COLLAYER
	{
		Col_Player,
		Col_Monster,
		Col_MonsterParry,
		Col_PlayerAttack,
		Col_PlayerHardAttack,
		Col_PlayerDodge,
		Col_MonsterAttack,
		Col_MonsterHardAttack,
		Col_Environment,
		Col_End
	};
}

extern HINSTANCE	g_hInst;
extern HWND			g_hWnd;

extern float g_TimeScale_Player;
extern float g_TimeScale_Monster;

using namespace Client;


