#pragma once

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN                           
#include <windows.h>
#include "resource.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <commdlg.h>

#include <Engine/global.h>
#ifdef _DEBUG
#pragma comment(lib, "Engine/Engine_debug.lib")
#else
#pragma comment(lib, "Engine/Engine.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "Script/Script_debug.lib")
#else
#pragma comment(lib, "Script/Script.lib")
#endif
