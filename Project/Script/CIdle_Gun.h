#pragma once
#include <Engine/CState.h>

class MonsterGunScript;

class CIdle_Gun
	: public CState
{
private:
	std::wstring      m_currentAnimName;
	MonsterGunScript* m_pScript;

public:
	void Update() override;
public:
	void Enter() override;
	void Exit() override;
	CLONE(CIdle_Gun);

public:
	CIdle_Gun(MonsterGunScript* pScript);
	virtual ~CIdle_Gun();
};
