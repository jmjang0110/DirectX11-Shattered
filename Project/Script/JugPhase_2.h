#pragma once
#include <Engine\CState.h>

class BossJugCombatMgrScript;
class CAnimator3D;
class CFSM;
class CGameObject;

class JugPhase_2 : public CState
{
private:
	BossJugCombatMgrScript* m_pCombatMgr;
	CFSM*                   m_pBossFSM;
	CAnimator3D*            m_pBossAnimator;

	vector<CGameObject*> m_vecColumnFlames;
	vector<CGameObject*> m_vecEnergyBalls;

	UINT m_iPrevAttackPattern;
	UINT m_iAttackPattern;

	float m_fIdleTime;
	float m_fAttackTime;
	float m_fRotTimer;
	int   m_iRotCounter;

	float m_fDMG[4];

	bool m_bAttackProceeding;
	bool m_bRot;


private:
	void RotTowardPlayer();
	void ChangePattern();

	void Attack_1();	// ºû ±âµÕ °ø°Ý
	void Attack_2();	// ¸ÁÄ¡ °ø°Ý
	void Attack_3();	// Åõ»çÃ¼ °ø°Ý

public:
	void         SetDamage(int _idx, float _dmg);
	virtual void Init();
	virtual void Enter();
	virtual void Exit();

	virtual void Update();
	virtual void LateUpdate();

	CLONE(JugPhase_2)

public:
	JugPhase_2();
	JugPhase_2(const JugPhase_2& _origin);
	virtual ~JugPhase_2();
};
