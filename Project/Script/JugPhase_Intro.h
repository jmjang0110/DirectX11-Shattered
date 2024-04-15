#pragma once
#include <Engine\CState.h>

class BossJugCombatMgrScript;

class JugPhase_Intro : public CState
{
private:
	BossJugCombatMgrScript* m_pCombatMgr;

public:
	virtual void Init();
	virtual void Enter();
	virtual void Exit();

	virtual void Update();
	virtual void LateUpdate();


	CLONE(JugPhase_Intro)
public:
	JugPhase_Intro();
	JugPhase_Intro(const JugPhase_Intro& _origin);
	virtual ~JugPhase_Intro();
};
