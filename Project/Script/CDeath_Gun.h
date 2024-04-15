#pragma once
#include <Engine/CState.h>

class CDeath_Gun
	: public CState
{
public:
	void Enter() override;
	void Exit() override;
	CLONE(CDeath_Gun);

public:
	CDeath_Gun();
	virtual ~CDeath_Gun();
};
