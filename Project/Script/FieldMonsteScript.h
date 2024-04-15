#pragma once
#include <Engine/CScript.h>

#include "FieldM_StateMgr.h"

class CGameObject;
class BossJugCombatMgrScript;

enum class FieldMonsterType
{
	HOMONCULUS,
	DEUXIEMIE,
	NONE
};

class FieldMonsteScript :
	public CScript
{
private:
	FieldM_StateMgr* m_pMonsterMgr;
	Vec3 m_vColliderSize;

	CGameObject* m_pPlayer;
	CGameObject* m_pOwnerMonster;

	float m_fDetachRange;
	float m_fAttackRange;
	float m_fCurDistance;

	float m_fFullHP;
	float m_fCurHP;

	bool	m_bIsChasing;

	bool m_bCurAnimationDone;
	bool m_bChasingON;

	FieldMonsterType m_eMonsterType;

	BossJugCombatMgrScript* m_pCombatMgr;
	bool                    m_bCanHit;
	bool                    m_bHit;
	float                   m_fEffectTime;

private:

	float m_fRotateSpeed;
	int m_iRotateDir;
	bool m_bRotateFinish;
	float m_fMoveAccRot; // 회전해야할 총 각도 ( 0 ~ 360 )



public:
	virtual void start();
	virtual void update();
	virtual void lateupdate();

	virtual void OnCollisionEnter(CGameObject* _OtherObject) override;
	virtual void OnCollision(CGameObject* _OtherObject) override;
	virtual void OnCollisionExit(CGameObject* _OtherObject) override;

public:
	void SetMonsterFullHP(float _hp) { m_fFullHP = _hp; }

	void SetMonsterHP(float _curHP) { m_fCurHP = _curHP; }
	float GetMonsterHP() { return m_fCurHP; }

	void SetHitStateOn(bool _TorF) { m_bHit = _TorF; }
	bool GetHitOn() { return m_bHit; }

	void SetDetectRange(float _range) { m_fDetachRange = _range; }
	void SetAttackRange(float _range) { m_fAttackRange = _range; }
	void SetColliderSize(Vec3 _size) { m_vColliderSize = _size; }
	void SetCurAnimationDone() { m_bCurAnimationDone = true; }

	void SetFieldMonsterType(FieldMonsterType _type) { m_eMonsterType = _type; }
	FieldMonsterType GetFieldMonsterType() { return m_eMonsterType; }

	FieldM_StateMgr* GetMonsterMGR() { return m_pMonsterMgr; }
	CGameObject* GetPlayer() { return m_pPlayer; }

public:
	bool DetectPlayer();
	void MonsterRotation(Vec3 _vPos);

	// _vDir 방향으로 돈다  
	void RotateY(Vec3 _vDir);
	float GetAngleRadian(Vec3 _v1, Vec3 _v2);

	void PeaceStateRotation();

public:
	CLONE(FieldMonsteScript)

	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	FieldMonsteScript(const FieldMonsteScript& _origin);
	FieldMonsteScript();
	virtual ~FieldMonsteScript();
};

