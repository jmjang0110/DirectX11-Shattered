#pragma once
#include <Engine/CState.h>

class CObjectMgrState_CutScene : public CState
{
private:
	void EnableSceneObject();

public:
	void Enter() override;
	void Exit() override;
	void LateUpdate() override;

public:
	CLONE(CObjectMgrState_CutScene);
	CObjectMgrState_CutScene();
	virtual ~CObjectMgrState_CutScene();
};
