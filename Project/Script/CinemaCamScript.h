#pragma once
#include <Engine/CScript.h>
#include "CatmullRomSpline.h"


class CGameObject;

class CinemaCamScript
	: public CScript
{
private:
	CGameObject* m_pTargetObject;
	int          m_iDir   = 1;
	float        m_fSpeed = 0.5f;

	bool                     m_bLoopRepeat;  // [0] - > [1] - > [2] ---...[0]
	vector<CatmullRomSpline> m_vecCatmullRom;

	bool m_bFinish = false;

public:
	void SetLoopRepeat(bool _b) { m_bLoopRepeat = _b; }
	void SetTargetObject(CGameObject* _pObj) { m_pTargetObject = _pObj; }

	void AddCatmullRomSpline(CatmullRomSpline _tCatmullRom) { m_vecCatmullRom.push_back(_tCatmullRom); }
	Vec3 CalCulateCatmullRom(Vec3 _p0, Vec3 _p1, Vec3 _p2, Vec3 _p3, float _fLerp);
	void ClearCatmullRom();
	void ResetCatmullRom();

	bool  IsFinish() const { return m_bFinish; }
	void  UpdatePosition();
	void  UpdateRotation();
	bool  RotateCamera(Vec3 _vFrom, Vec3 _vTo);
	float GetAngleRadian(Vec3 _v1, Vec3 _v2);

	// Pos0 : 보간 시작 지점 / Pos1 : 보간 끝 지점 
	void CreateCatmullRomRoute(Vec3 _vPos0, vector<Vec3> _vecMainPos, Vec3 _vPos1);
	void CreateStartMap02Route(Vec3 _vStartPos);

public:
	virtual void start();
	virtual void update();
	virtual void lateupdate();

	virtual void OnCollisionEnter(CGameObject* _OtherObject) override;
	virtual void OnCollision(CGameObject* _OtherObject) override;
	virtual void OnCollisionExit(CGameObject* _OtherObject) override;

public:
	virtual void SaveToScene(FILE* _pFile) override;
	virtual void LoadFromScene(FILE* _pFile) override;

	CLONE(CinemaCamScript)

public:
	CinemaCamScript(const CinemaCamScript& _origin);
	CinemaCamScript();
	virtual ~CinemaCamScript();
};
