#pragma once
#include <Engine/CScript.h>

class MonsterSearchScript final
	: public CScript
{
private:
	bool         m_isPlayerInRange;
	CGameObject* m_pTarget;
public:
	bool         IsPlayerInRange() const { return m_isPlayerInRange; }
	CGameObject* GetTarget() const { return m_pTarget; }
	void         SetRange(bool enable) { m_isPlayerInRange = enable; }
public:
	void OnCollisionEnter(CGameObject* _OtherObject) override;
	void OnCollisionExit(CGameObject* _OtherObject) override;
public:
	CLONE(MonsterSearchScript);
public:
	MonsterSearchScript();
	virtual ~MonsterSearchScript();
};
