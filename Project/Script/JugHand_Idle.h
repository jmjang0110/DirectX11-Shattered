#pragma once
#include <Engine\CState.h>

class CAnimation3D;

class JugHand_Idle :
	public CState
{
public:
	CAnimation3D* m_pAnimation;
	float			m_fLerfTime;

public:
	void Enter() override;
	void Exit() override;

public:
	void Update() override;


public:
	CLONE(JugHand_Idle);
	JugHand_Idle();
	virtual ~JugHand_Idle();
};

