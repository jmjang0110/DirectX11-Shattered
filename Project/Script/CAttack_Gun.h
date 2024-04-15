#pragma once
#include <Engine/CState.h>

class CAttack_Gun
	: public CState
{
public:
	void Enter() override;
	void Exit() override;
	CLONE(CAttack_Gun);

public:
	CAttack_Gun();
	virtual ~CAttack_Gun();
};
