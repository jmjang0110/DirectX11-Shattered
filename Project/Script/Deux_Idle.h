#pragma once

#include <Engine\CState.h>

class CAnimation3D;
class FieldM_StateMgr;

class Deux_Idle :
	public CState
{
public:
	CAnimation3D* m_pAnimation;
	FieldM_StateMgr* m_pOwnerMGR;

public:
	void Enter() override;
	void Exit() override;

public:
	void Update() override;


public:
	CLONE(Deux_Idle);
	Deux_Idle();
	virtual ~Deux_Idle();
};

