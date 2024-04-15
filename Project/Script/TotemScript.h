#pragma once

#include <Engine/CScript.h>

class BossJugCombatMgrScript;

class TotemScript : public CScript
{
private:
	float	m_fTotemFullHP;
	float	m_fTotemCurHP;

	bool	m_bHitOn;
	bool	m_bPrevHitOn;

	float	m_fPaperburnTime;

	Vec3 m_pPlayerPrevPos;
	Vec3 m_pPlayerCurPos;

	CGameObject* m_pPlayer;

	float m_fTimer;
	int m_iShaker;
	Vec3 m_vOriginPos;


	BossJugCombatMgrScript* m_pCombatMgr;
	bool                    m_bCanHit;
	bool                    m_bHit;
	float                   m_fEffectTime;

	void HitEffect();

public:
	virtual void start();
	virtual void update();
	virtual void lateupdate();

	virtual void OnCollisionEnter(CGameObject* _OtherObject) override;
	virtual void OnCollision(CGameObject* _OtherObject) override;
	virtual void OnCollisionExit(CGameObject* _OtherObject) override;

	CLONE(TotemScript);

public:

	TotemScript();
	virtual ~TotemScript();
};
