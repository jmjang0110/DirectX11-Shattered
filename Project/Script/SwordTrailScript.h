#pragma once
#include <Engine/CScript.h>

/*
	무기 트레일 스크립트를 소유한 오브젝트의
	Mesh - Vertex Desc.Usage 가 D3D11_USAGE_DYNAMIC 이어야 합니다.


*/
class SwordTrailScript :
	public CScript
{

private:
	CGameObject* m_pTarget; // L"Sword_Trail

	Vec3			m_vForward;
	Vec3			m_vBackward;

	Vec3			m_vStartPos;
	Vec3			m_vEndPos;

	Vec3			m_vCurPos_World;
	Vec3			m_vPrevPos_World;

	Vec3			m_vCurPos_Local;
	Vec3			m_vPrevPos_Local;

	bool			m_bStart;
	int				m_iUpdateVtx_Cnt;
	int				m_iUpdateVtx_Max;


private:
	Vec3			m_vSword_Pos;
	Vec3			m_vSword_Rot;
	Vec3			m_vSword_Forward;

	bool m_bStart_Setting = false;
	bool m_bTest = false;
	bool m_bFinish_Start = false;

	int m_iShrink_Idx = 0;

public:
	void SetTarget(CGameObject* _pTarget) { m_pTarget = _pTarget; }

	void UpdateVertexInfo_Start();
	void UpdateVertexInfo_End();

	void Reset();



	void setPos(Vec3 _vPos) { m_vSword_Pos = _vPos; }
	void SetRot(Vec3 _vRot) { m_vSword_Rot = _vRot; }
	void SetForward(Vec3 _vForward) { m_vSword_Forward = _vForward; }
	void TrailStart() { m_bStart = true; }
	bool isStart() { return m_bStart; }

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

	CLONE(SwordTrailScript)

public:
	SwordTrailScript();
	SwordTrailScript(const SwordTrailScript* _origin);
	virtual ~SwordTrailScript();
};

