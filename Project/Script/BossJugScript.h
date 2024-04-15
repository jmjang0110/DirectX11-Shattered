#pragma once
#include <Engine\CScript.h>

class CAnimatior3D;
class CState;
class CGameObject;

class BossJugScript
	: public CScript
{
private:
	CFSM*        m_pBossFSM;
	CAnimator3D* m_pBossAnimator;

	string m_strCurState;
	string m_strCurAnimName;

	float m_fHP;
	float m_fMaxHP;

	float m_fDamage;

public:
	float        GetHP() { return m_fHP; }
	float        GetMaxHP() { return m_fMaxHP; }
	CFSM*        GetBossFSM() { return m_pBossFSM; }
	CAnimator3D* GetBossAnimator() { return m_pBossAnimator; }

	void SetHP(float _hp);
	void SetMaxHP(float _hp) { m_fMaxHP = _hp; }

	void GetHit(float _fDamageRate);

	void Init();
	void InitState();

	virtual void start();
	virtual void update();
	virtual void lateupdate();

	virtual void OnCollisionEnter(CGameObject* _OtherObject) override;
	virtual void OnCollision(CGameObject* _OtherObject) override;
	virtual void OnCollisionExit(CGameObject* _OtherObject) override;

	CLONE(BossJugScript);

	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	BossJugScript();
	BossJugScript(const BossJugScript& _origin);
	virtual ~BossJugScript();
};
