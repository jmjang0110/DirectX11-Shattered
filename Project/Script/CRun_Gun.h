#pragma once
#include <Engine/CState.h>

class MonsterGunScript;

class CRun_Gun
	: public CState
{
public:
	MonsterGunScript* m_pScript;
	float             m_runSpeed;
	float             m_rotationSpeed;
	bool              m_rotationFinish;
public:
	void Update() override;
	void Enter() override;
	void Exit() override;
	CLONE(CRun_Gun);

public:
	CRun_Gun(MonsterGunScript* pScript);
	virtual ~CRun_Gun();
};
