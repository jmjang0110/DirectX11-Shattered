#include "pch.h"
#include "CObjectMgrState_CutScene.h"

#include <Engine/CCamera.h>
#include <Engine/CFSM.h>

#include "CinemaCamScript.h"
#include "CObjectManager.h"

CObjectMgrState_CutScene::CObjectMgrState_CutScene(): CState{L"CUT_SCENE"} {}
CObjectMgrState_CutScene::~CObjectMgrState_CutScene() = default;

void CObjectMgrState_CutScene::Enter()
{


}

void CObjectMgrState_CutScene::Exit()
{
	CObjectManager::GetInst()->GetPlayerCamera()->Camera()->SetCameraAsMain();
	CKeyMgr::GetInst()->EnableAll(true);
}

void CObjectMgrState_CutScene::LateUpdate()
{
	if (CObjectManager::GetInst()->GetCinemaCam()->GetScript<CinemaCamScript>()->IsFinish())
	{
		CObjectManager::GetInst()->FSM()->ChangeState(L"IDLE");

	}
}
