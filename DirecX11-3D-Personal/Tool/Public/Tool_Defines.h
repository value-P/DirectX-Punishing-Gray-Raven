#pragma once

namespace Tool
{
	enum LEVEL { LEVEL_STATIC,LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;
}

extern HINSTANCE	g_hInst;
extern HWND			g_hWnd;
extern float		g_TimeScale;

using namespace Tool;


