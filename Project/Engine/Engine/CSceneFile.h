#pragma once
#include "CRes.h"

class CScene;

class CSceneFile
	: public CRes
{
public:
	int Load(const wstring& _strFilePath) override;

public:
	CSceneFile();
	virtual ~CSceneFile();
};
