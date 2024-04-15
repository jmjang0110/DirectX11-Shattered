#pragma once
#include <Engine/CState.h>

class ObjMgrState_LOADING
	: public CState
{
private:
	void EnableSceneObject();

public:
	void Enter() override;
	void Exit() override;
	void LateUpdate() override;

public:
	CLONE(ObjMgrState_LOADING);
	ObjMgrState_LOADING();
	virtual ~ObjMgrState_LOADING();
};
