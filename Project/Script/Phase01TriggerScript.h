#pragma once
#include <Engine\CScript.h>

class BossJugCombatMgrScript;

class Phase01TriggerScript : public CScript
{
private:
	BossJugCombatMgrScript* m_pCombatMgr;

public:
	virtual void start();
	virtual void update();
	virtual void lateupdate();

	virtual void OnCollisionEnter(CGameObject* _OtherObject) override;
	virtual void OnCollision(CGameObject* _OtherObject) override;
	virtual void OnCollisionExit(CGameObject* _OtherObject) override;

	CLONE(Phase01TriggerScript)

	Phase01TriggerScript();
	virtual ~Phase01TriggerScript();
};
