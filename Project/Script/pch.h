#pragma once

#ifdef _DEBUG
#pragma comment(lib, "Engine/Engine_debug.lib")
#else
#pragma comment(lib, "Engine/Engine.lib")
#endif

#include <Engine/global.h>
#include "CScriptMgr.h"
#include "GameDefine.h"

#include <random>
