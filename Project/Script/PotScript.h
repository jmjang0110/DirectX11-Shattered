#pragma once
#include <Engine\CScript.h>

class BossJugCombatMgrScript;

class PotScript : public CScript
{
private:
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

	virtual void SaveToScene(FILE* _pFile) override;
	virtual void LoadFromScene(FILE* _pFile) override;

	CLONE(PotScript)

	PotScript();
	PotScript(const PotScript& _origin);
	~PotScript();
};
