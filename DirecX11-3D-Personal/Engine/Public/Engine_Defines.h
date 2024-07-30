#pragma once

#pragma warning(disable:4251)
#define DIRECTINPUT_VERSION 0x0800

// dx11 헤더
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <Effects11\d3dx11effect.h>
#include <TK\DDSTextureLoader.h>
#include <TK\WICTextureLoader.h>
#include <TK\SimpleMath.h>
#include <TK\ScreenGrab.h>
#include <TK\PrimitiveBatch.h>
#include <TK\VertexTypes.h>
#include <TK\Effects.h>
#include <TK\SpriteBatch.h>
#include <TK\SpriteFont.h>
#include <dinput.h>
using namespace DirectX;
using namespace DirectX::SimpleMath;

// cpp 헤더
#include <memory>
#include <vector>
#include <map>
#include <queue>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <random>
using namespace std;

// 사용자 정의 헤더
#include "Engine_Function.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Enums.h"

// ImGui 헤더
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// Json
#include "json\json.h"

// Fmod
#include "Fmod\fmod.h"
#include "Fmod\fmod.hpp"

using namespace Engine;

#include<wrl.h>
#include<wrl/client.h>
using namespace Microsoft::WRL;

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

