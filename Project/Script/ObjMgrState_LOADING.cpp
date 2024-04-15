#include "pch.h"
#include "ObjMgrState_LOADING.h"

#include <Engine/CCollider3D.h>
#include <Engine/CFSM.h>
#include <Engine/CGameObject.h>
#include <Engine/CRenderEffectMgr.h>
#include <Engine/CCamera.h>
#include "BossJugCombatMgrScript.h"
#include "CObjectManager.h"
#include "CinemaCamScript.h"
#include <Engine/CNaviAgent.h>

ObjMgrState_LOADING::ObjMgrState_LOADING()
	: CState{L"LOADING"} {}

ObjMgrState_LOADING::~ObjMgrState_LOADING() {}


void ObjMgrState_LOADING::Enter()
{
	CRenderEffectMgr::GetInst()->ChangeBurnColor(BLACK);
	CRenderEffectMgr::GetInst()->Enable_FadeInPaperBurn();
	
	
}


void ObjMgrState_LOADING::LateUpdate()
{
	if (CRenderEffectMgr::GetInst()->IsEnable_FadeInPaperBurn() && CRenderEffectMgr::GetInst()->IsFadeInFinished())
	{
		CRenderEffectMgr::GetInst()->UseAllBlack(true);
		CRenderEffectMgr::GetInst()->Enable_FadeOutPaperBurn(2.f);
		EnableSceneObject();
		CGameObject* pBoss = new CGameObject;
		pBoss->SetName(L"BOSS_COMBAT");
		pBoss->AddComponent(new CTransform);
		pBoss->AddComponent(new BossJugCombatMgrScript);
		CSceneMgr::GetInst()->SpawnObject(pBoss, GAME::LAYER::OBJECT_MGR);
		pBoss->GetScript<BossJugCombatMgrScript>()->SpawnStage();
		const MAP_TYPE type       = CObjectManager::GetInst()->GetCurrentMapType();
		const Vec3     startPoint = CObjectManager::GetInst()->GetStartingPoint(type);

		CGameObject* pPlayer = CObjectManager::GetInst()->GetPlayer();
		if (pPlayer)
		{
			pPlayer->Transform()->SetRelativePos(startPoint);
			pPlayer->NaviAgent()->finalupdate();
		}
		CObjectManager::GetInst()->GetCinemaCam()->Activate();
		CObjectManager::GetInst()->GetCinemaCam()->Camera()->SetCameraAsMain();
		CObjectManager::GetInst()->GetCinemaCam()->GetScript<CinemaCamScript>()->
		                           CreateStartMap02Route(CObjectManager::GetInst()->GetPlayer()->Transform()->
		                                                 GetRelativePos());
	}
	else if (CRenderEffectMgr::GetInst()->IsEnable_FadeOutPaperBurn() && CRenderEffectMgr::GetInst()->
	         IsFadeOutFinished())
	{
		GetOwner()->FSM()->ChangeState(L"CUT_SCENE");
	}
}

void ObjMgrState_LOADING::Exit() { }

void ObjMgrState_LOADING::EnableSceneObject()
{
	const MAP_TYPE type = CObjectManager::GetInst()->GetCurrentMapType();
	for (auto& pGameObject : CObjectManager::GetInst()->GetSceneObjectList(type))
	{
		pGameObject->Deactivate();
	}
	for (auto& pGameObject : CObjectManager::GetInst()->GetDontDestroyList())
	{
		pGameObject->Activate();
	}

	int nextMapType = (int)type + 1;
	if ((MAP_TYPE)nextMapType != MAP_TYPE::END)
	{
		CObjectManager::GetInst()->SetMapType((MAP_TYPE)nextMapType);
		for (auto& pGameObject : CObjectManager::GetInst()->GetSceneObjectList((MAP_TYPE)nextMapType))
		{
			pGameObject->Activate();
		}

		CObjectManager::GetInst()->SetMapType((MAP_TYPE)nextMapType);
	}
}
