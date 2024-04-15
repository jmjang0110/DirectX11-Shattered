#pragma once
#include <Engine/CScript.h>

#include "MonsterSearchScript.h"

class MonsterSearchScript;
class CState;

class MonsterGunScript final
	: public CScript
{
private:
	int                  m_hp;
	float                m_radius;
	float                m_prepareRange;
	float                m_attackRange;
	MonsterSearchScript* m_pSearch;

private :
	void TestCode(int a);
	void TestCode2(int a, float b);
	void InitState();
	void MakeSearchObject();
public:
	const MonsterSearchScript& GetSearchScript() const { return *m_pSearch; }
	float                      GetPrepareRange() const { return m_prepareRange; }
	float                      GetAttackRange() const { return m_attackRange; }
public:
	void start() override;
	void update() override;

public:
	void OnCollisionEnter(CGameObject* _OtherObject) override;

public:
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;
public:
	CLONE(MonsterGunScript);
	MonsterGunScript();
	virtual ~MonsterGunScript();
};
