#pragma once
#include <Engine/CState.h>

class CWalk_Gun
	: public CState
{
public:
	void Enter() override;
	void Exit() override;
	CLONE(CWalk_Gun);

public:
	CWalk_Gun();
	virtual ~CWalk_Gun();
};
