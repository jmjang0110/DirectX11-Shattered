#pragma once
#include <Engine\CScript.h>

#include "HandStateMgrScript.h"

class BossJugHandScript :
	public CScript
{
private:
	Vec3            m_vDirection;
	Vec3            m_vPrevDirection;

	CGameObject*		m_pPlayer;

	wstring					m_sCurState;

	float					m_fRunningTime;
	float					m_fSpeed;

	int						m_iOwnerHandIdx;
	int						m_iCurAttackHandIdx;

	bool					m_bAllAttackIsDone;
	bool					m_bGenStateDone;
	bool					m_bAttackStateDone;
	bool					m_bVanishStateDone;

	bool					m_bAttackRepeat;
	bool				m_bPaperburnOn;

public:
	// [ GET ]
	int GetHandIndexNumber() { return m_iOwnerHandIdx; }
	int GetCurAttackHandIdx() { return m_iCurAttackHandIdx; }
	float GetRunningTime() { return m_fRunningTime; }
	float GetSpeed() { return m_fSpeed; }
	Vec3 GetCurDir() { return m_vDirection; }
	Vec3 GetPlayerPosition();
	bool GetAttackRepeat() { return m_bAttackRepeat; }
	wstring GetCurState() { return m_sCurState; }


	// [ SET ]
	void SetRunningTime(float _time) { m_fRunningTime = _time; }
	void SetMonsterDir(Vec3 _dir) { m_vDirection = _dir; }
	void SetMonsterSpeed(float _speed) { m_fSpeed = _speed; }
	void SetGenStateDone(bool _TorF) { m_bGenStateDone = _TorF; }
	void SetAttackStateDone(bool _TorF) { m_bAttackStateDone = _TorF; }
	void SetVanishStateDone(bool _TorF) { m_bVanishStateDone = _TorF; }
	void SetAllAttackDone(bool _TorF) { m_bAllAttackIsDone = _TorF; }
	void SetAttackRepeat(bool _TorF) { m_bAttackRepeat = _TorF; }
public:
	void InitMonsterStat();


public:
	virtual void start();
	virtual void update();
	virtual void lateupdate();

	virtual void OnCollisionEnter(CGameObject* _OtherObject)	override;
	virtual void OnCollision(CGameObject* _OtherObject)			override;
	virtual void OnCollisionExit(CGameObject* _OtherObject)		override;

public:
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(BossJugHandScript);
	BossJugHandScript();
	BossJugHandScript(const BossJugHandScript& _origin);
	virtual ~BossJugHandScript();


};

