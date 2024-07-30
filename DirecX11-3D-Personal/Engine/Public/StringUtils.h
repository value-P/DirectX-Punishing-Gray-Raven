#pragma once
#include "Engine_Defines.h"

class ENGINE_DLL CStringUtils
{
public:
	static wstring toWstring(const _char* pStr);
	static wstring toWstring(const string& str);
};