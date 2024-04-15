#pragma once
#include "CRes.h"
#include "Ptr.h"
#include "CMaterial.h"
#include "CMesh.h"

class CMeshData
	: public CRes
{
	Ptr<CMesh>             m_pMesh;
	vector<Ptr<CMaterial>> m_vecMtrl;

public:
	static vector<CMeshData*> LoadFromFBX(const wstring& _strFilePath);

	int Save(const wstring& _strFilePath) override;
	int Load(const wstring& _strFilePath) override;

	CGameObject* Instantiate();
	Ptr<CMesh>   GetMesh() { return m_pMesh; }

public:
	CMeshData();
	virtual ~CMeshData();
};
