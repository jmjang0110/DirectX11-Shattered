#pragma once

#include <Engine\CState.h>

class CAnimation3D;
class FieldM_StateMgr;

class Deux_Walk :
	public CState
{
public:
	CAnimation3D*		m_pAnimation;
	FieldM_StateMgr*	m_pOwnerMGR;

	float				m_fSpeed;			// 속도 

	bool				m_bDetectTarget;

	Vec3				m_vPatrolPoint;		// Patrol 기준점 ( 고정 )	
	float				m_fPatrolRange;		// Patrol 범위 
	Vec3				m_vPatrolTargetPos;	// Patrol target Pos 
	bool				m_bPatrolFinish;	// Patrol Target Pos 에 도착 했는지 여부

public:
	void Enter() override;
	void Exit() override;

public:
	void Update() override;
	void UpdatePos();

	void Trace(CGameObject* _pTarget);
	void Patrol();
	void UpdatePatrolPoint(Vec3 _vPos);
	void UpdatePatrolTargetPos();


	void Rotate(Vec3 vDir);

public:
	CLONE(Deux_Walk);
	Deux_Walk();
	virtual ~Deux_Walk();
};

