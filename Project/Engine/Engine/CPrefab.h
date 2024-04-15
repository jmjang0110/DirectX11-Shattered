#pragma once
#include "CRes.h"

class CGameObject;

class CPrefab
	: public CRes
{
private:
	CGameObject* m_pProtoObj;

public:
	typedef void (* SaveFunc)(CPrefab*, const wstring&, bool isBinary);
	typedef int (*  LoadFunc)(CPrefab*, const wstring&, bool isBinary);
	static SaveFunc m_pSaveFunc;
	static LoadFunc m_pLoadFunc;

public:
	CGameObject* Instantiate() const;
	CGameObject* GetProto() const { return m_pProtoObj; }
	void         SetProto(CGameObject* _pProto) { m_pProtoObj = _pProto; }

public:
	int Save(const wstring& _strFilePath) override;
	int Load(const wstring& _strFilePath) override;

public:
	CPrefab();
	CPrefab(CGameObject* _pProtoObj);
	virtual ~CPrefab();
};
