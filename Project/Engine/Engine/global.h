#pragma once

#include <wrl/client.h>
using namespace Microsoft::WRL;

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

using namespace::DirectX;
using namespace::DirectX::PackedVector;

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxguid")

//Direct2D
#include <d2d1.h>
#include <dwrite.h>
#pragma comment(lib,"d2d1")
#pragma comment(lib,"dwrite")


#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

#include <string>
using std::string;
using std::wstring;

#include <vector>
using std::vector;

#include <list>
using std::list;

#include <map>

using std::map;
using std::make_pair;

#include <locale>

#include <assert.h>
#include <filesystem>
#include <bitset>
#include <array>

// Fbx Loader
#include <FBXLoader/fbxsdk.h>

//FBX
#ifdef _DEBUG
#pragma comment(lib, "FBXLoader/x64/debug/libfbxsdk-md.lib")
#else
#pragma comment(lib, "FBXLoader/x64/release/libfbxsdk-md.lib")
#endif

//YAML
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#if _DEBUG
#pragma comment(lib,"yaml-cpp/yaml-cppd")
#else
#pragma comment(lib,"yaml-cpp/yaml-cpp")
#endif

//MAGIC_ENUM
#include <MagicEnum/magic_enum.hpp>

#include "define.h"
#include "struct.h"
#include "extern.h"
#include "singleton.h"
#include "func.h"
#include "CLog.h"
#include "CTimeMeasure.h"
