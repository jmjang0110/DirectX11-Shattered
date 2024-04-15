#pragma once
#include <Engine/CScript.h>

class CGameObject;

class BossJugCombatMgrScript : public CScript
{
private:
	CFSM*  m_pPhaseFSM;
	string m_strCurState;

	CGameObject*       m_pStage;
	CGameObject*       m_pNaviMap;
	CGameObject*       m_pJug;
	CGameObject*       m_pHammer;
	CGameObject*       m_pJugHandMgr;
	CGameObject*       m_pPlayer;
	CGameObject*       m_pPot;
	CGameObject*       m_pPhase01Trigger;
	list<CGameObject*> m_listTotem;

	CGameObject* m_pStageInnerCollider;
	CGameObject* m_pStageOuterCollider;

	bool m_bStartCombat;
	bool m_bPhase1Enter;

public:
	void SpawnStage();
	void InitState();
	void CheckPhase();
	void TotemDeadCheck();
	void Phase1Enter() { m_bPhase1Enter = true; }

	// [ GET / SET ]
	CGameObject* GetJug() { return m_pJug; }
	CGameObject* GetHammer() { return m_pHammer; }
	CGameObject* GetJugHand() { return m_pJugHandMgr; }
	CGameObject* GetPlayer() { return m_pPlayer; }
	CGameObject* GetPot() { return m_pPot; }
	CFSM*        GetPhaseFSM() { return m_pPhaseFSM; }
	wstring      GetCurPhase() { return ToWString(m_strCurState); }

	void start() override;
	void update() override;
	void lateupdate() override;

	CLONE(BossJugCombatMgrScript)

	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	BossJugCombatMgrScript();
	virtual ~BossJugCombatMgrScript();
};
