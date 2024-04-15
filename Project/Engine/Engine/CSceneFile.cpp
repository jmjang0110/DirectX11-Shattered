#include "pch.h"
#include "CSceneFile.h"

CSceneFile::CSceneFile()
	: CRes(RES_TYPE::SCENEFILE) {}

CSceneFile::~CSceneFile() = default;

int CSceneFile::Load(const wstring& _strFilePath)
{
	return S_OK;
}
