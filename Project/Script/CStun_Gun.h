#pragma once
#include <Engine/CState.h>

class CStun_Gun
	: public CState
{
public:
	void Enter() override;
	void Exit() override;
	CLONE(CStun_Gun);

public:
	CStun_Gun();
	virtual ~CStun_Gun();
};
