#pragma once
#include "CComponent.h"

class CNaviAgent : public CComponent
{
private:
	CNaviMap*   m_pTargetNaviMap;
	CRigidBody* m_pRigidBody;

	Vec3 m_vOffsetSize;
	Vec3 m_vOffsetPos;
	Vec3 m_vPrevPos;
	Vec4 m_vDebugAgentColor;
	int  m_iCurCellIndex;
	bool m_bGravity;

private:
	void FindCurIndex();
	bool CheckLinkedCell();
	Vec3 SetYPos(float _fDist);

public:
	void      SetNaviMap(CNaviMap* _pNaviMap);
	void      SetOffsetSize(Vec3 _vSize) { m_vOffsetSize = _vSize; }
	void      SetOffsetPos(Vec3 _vPos) { m_vOffsetPos = _vPos; }
	void      SetGravity(bool _bUse) { m_bGravity = _bUse; }
	CNaviMap* GetNaviMap() { return m_pTargetNaviMap; }
	Vec3      GetOffsetSize() { return m_vOffsetSize; }
	Vec3      GetOffsetPos() { return m_vOffsetPos; }
	UINT      GetCurCellIndex() { return m_iCurCellIndex; }
	bool      GetGravity() { return m_bGravity; }

	void update() override;
	void lateupdate() override;
	void finalupdate() override;
	void finalupdate_debug() override;
	void render_debug() override;

	void UpdateData() override;

public:
	CLONE(CNaviAgent);
	CNaviAgent();
	CNaviAgent(const CNaviAgent& _origin);
	virtual ~CNaviAgent();
};
