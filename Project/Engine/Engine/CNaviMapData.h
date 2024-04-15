#pragma once
#include "CRes.h"

class CNaviCell
{
private:
	int         m_iIDX;
	Vtx         m_arrVTX[3];
	vector<int> m_vecLinkedCell;

public:
	const Vtx*         GetVTX() { return m_arrVTX; }
	const vector<int>& GetLinkedCellIDX() const { return m_vecLinkedCell; }

	CNaviCell();
	~CNaviCell();

	friend class CNaviMapData;
};

class CNaviMapData : public CRes
{
private:
	CMesh*             m_pNavMesh;
	vector<CNaviCell*> m_vecNaviCell;

public:
	Ptr<CMesh> GetNaviMesh() const { return m_pNavMesh; }
	CNaviCell* GetCell(int _iIDX) const { return m_vecNaviCell[_iIDX]; }
	UINT       GetCellCount() const { return static_cast<UINT>(m_vecNaviCell.size()); }

	bool CreateFromObj(const wstring& _strRelativePath, const wstring& _strMeshPath);

	int Save(const wstring& _strFilePath) override;
	int Load(const wstring& _strFilePath) override;

private:
	void Bake(const vector<Vtx>& _vVTXBuffer, const vector<UINT>& _vIDXBuffer);

public:
	CNaviMapData();
	virtual ~CNaviMapData();
};
