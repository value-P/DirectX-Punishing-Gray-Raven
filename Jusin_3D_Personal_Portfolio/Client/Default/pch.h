// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#pragma once

// 여기에 미리 컴파일하려는 헤더 추가
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// c++ 라이브러리
#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <type_traits>

// C 표준 라이브러리
#include <crtdbg.h>

// 사용자 정의 헤더
#include "Client_Defines.h"
#include "GameInstance.h"
#include "FXPool.h"