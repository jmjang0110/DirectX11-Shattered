#pragma once
#include <Engine/CState.h>

class ObjMgrState_IDLE
	: public CState
{
private:
	bool m_bGameStart;
	bool m_bGameEnd;
	int  m_bDoorEnabled = false;

public:
	void Enter() override;
	void Exit() override;
	void LateUpdate() override;

public:
	CLONE(ObjMgrState_IDLE);
	ObjMgrState_IDLE();
	virtual ~ObjMgrState_IDLE();
};
