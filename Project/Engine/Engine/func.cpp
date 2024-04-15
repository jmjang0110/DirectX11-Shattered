#include "pch.h"

void SaveStringToFile(const string& _str, FILE* _pFile)
{
	const BYTE nameLength = static_cast<BYTE>(_str.length());
	fwrite(&nameLength, sizeof(BYTE), 1, _pFile);
	fwrite(_str.c_str(), sizeof(char), nameLength, _pFile);
}

void LoadStringFromFile(string& _str, FILE* _pFile)
{
	char   szBuffer[256] = {};
	size_t nameLength{};
	fread(&nameLength, sizeof(BYTE), 1, _pFile);
	fread(szBuffer, sizeof(char), nameLength, _pFile);

	_str = szBuffer;
}

void SaveWStringToFile(const wstring& _str, FILE* _pFile)
{
	const BYTE nameLength = static_cast<BYTE>(_str.length());
	fwrite(&nameLength, sizeof(BYTE), 1, _pFile);
	fwrite(_str.c_str(), sizeof(wchar_t), nameLength, _pFile);
}

void LoadWStringFromFile(wstring& _str, FILE* _pFile)
{
	wchar_t szBuffer[256] = {};
	size_t  nameLength{};
	fread(&nameLength, sizeof(BYTE), 1, _pFile);
	fread(szBuffer, sizeof(wchar_t), nameLength, _pFile);

	_str = szBuffer;
}


bool CloseEnough(const float& a, const float& b, const float& epsilon)
{
	return (epsilon > std::abs(a - b));
}

Vec3 DecomposeRotMat(const Matrix& _matRot)
{
	// _mat 을 분해 후 다시 행렬 만들기	
	Vec4 vMat[4];

	vMat[0] = Vec4(_matRot._11, _matRot._12, _matRot._13, _matRot._14);
	vMat[1] = Vec4(_matRot._21, _matRot._22, _matRot._23, _matRot._24);
	vMat[2] = Vec4(_matRot._31, _matRot._32, _matRot._33, _matRot._34);
	vMat[3] = Vec4(_matRot._41, _matRot._42, _matRot._43, _matRot._44);

	Vec3 vNewRot;
	if (CloseEnough(vMat[0].z, -1.0f))
	{
		float x = 0; //gimbal lock, value of x doesn't matter
		float y = XM_PI / 2;
		float z = x + atan2f(vMat[1].x, vMat[2].x);
		vNewRot = Vec3{x, y, z};
	}
	else if (CloseEnough(vMat[0].z, 1.0f))
	{
		float x = 0;
		float y = -XM_PI / 2;
		float z = -x + atan2f(-vMat[1].x, -vMat[2].x);
		vNewRot = Vec3{x, y, z};
	}
	else
	{
		//two solutions exist
		float y1 = -asinf(vMat[0].z);
		float y2 = XM_PI - y1;

		float x1 = atan2f(vMat[1].z / cosf(y1), vMat[2].z / cosf(y1));
		float x2 = atan2f(vMat[1].z / cosf(y2), vMat[2].z / cosf(y2));

		float z1 = atan2f(vMat[0].y / cosf(y1), vMat[0].x / cosf(y1));
		float z2 = atan2f(vMat[0].y / cosf(y2), vMat[0].x / cosf(y2));

		//choose one solution to return
		//for example the "shortest" rotation
		if ((std::abs(x1) + std::abs(y1) + std::abs(z1)) <= (std::abs(x2) + std::abs(y2) + std::abs(z2)))
		{
			vNewRot = Vec3{x1, y1, z1};
		}
		else
		{
			vNewRot = Vec3{x2, y2, z2};
		}
	}
	return vNewRot;
}

Matrix GetMatrixFromFbxMatrix(const FbxAMatrix& _mat)
{
	Matrix mat;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mat.m[i][j] = static_cast<float>(_mat.Get(i, j));
		}
	}
	return mat;
}


std::wstring ToWString(COMPONENT_TYPE _type)
{
	LOG_ASSERT(_type != COMPONENT_TYPE::END && _type!=COMPONENT_TYPE::SCRIPT, "INVALID COMPONENT TYPE");

	//강사님 버전
	/*static const wchar_t* szWString[static_cast<UINT>(COMPONENT_TYPE::END)] =
	{
		L"TRANSFORM",
		L"CAMERA",
		L"COLLIDER2D",
		L"COLLIDER3D",
		L"ANIMATOR2D",
		L"ANIMATOR3D",
		L"LIGHT2D",
		L"LIGHT3D",
		L"BOUNDINGBOX",
		L"FSM",
		L"RIGIDBODY",
		L"NAVIMAP",
		L"NAVIAGENT",
		L"UIPANEL",
		L"UIIMAGE",
		L"UITEXT",
		L"UIPROGRESSBAR",
		L"UIBUTTON",
		L"MESHRENDER",
		L"TILEMAP",
		L"PARTICLESYSTEM",
		L"LANDSCAPE",
		L"DECAL",
		L"SKYBOX",
	};
	return szWString[static_cast<UINT>(_type)];*/
	return ToWString(magic_enum::enum_name(_type).data());
}

std::string ToString(COMPONENT_TYPE _type)
{
	LOG_ASSERT(_type != COMPONENT_TYPE::END && _type!=COMPONENT_TYPE::SCRIPT, "INVALID COMPONENT TYPE");

	return magic_enum::enum_name(_type).data();
}

std::wstring ToWString(RES_TYPE _type)
{
	//강사님 버전
	/*static const wchar_t* szWString[static_cast<UINT>(RES_TYPE::END)] =
	{
		L"PREFAB",
		L"MESHDATA",
		L"MATERIAL",
		L"GRAPHICS_SHADER",
		L"COMPUTE_SHADER",
		L"MESH",
		L"TEXTURE",
		L"SOUND",
		L"SCENEFILE",
		L"NAVIMAP"
	};

	return szWString[static_cast<UINT>(_type)];*/
	LOG_ASSERT(_type != RES_TYPE::END, "INVALID COMPONENT TYPE");
	return ToWString(magic_enum::enum_name(_type).data());
}

std::string ToString(RES_TYPE _type)
{
	LOG_ASSERT(_type != RES_TYPE::END, "INVALID COMPONENT TYPE");
	return magic_enum::enum_name(_type).data();
}
