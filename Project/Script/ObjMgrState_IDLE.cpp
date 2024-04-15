#include "pch.h"
#include "ObjMgrState_IDLE.h"

#include <Engine/CCollider3D.h>
#include <Engine/CFSM.h>
#include <Engine/CGameObject.h>

#include "CObjectManager.h"
#include "DoorScript.h"
#include <Engine/CNaviAgent.h>

ObjMgrState_IDLE::ObjMgrState_IDLE()
	: CState{L"IDLE"}
	, m_bGameStart{false}
	, m_bGameEnd{false}
{
}

ObjMgrState_IDLE::~ObjMgrState_IDLE() = default;

void ObjMgrState_IDLE::Enter()
{
	/*const MAP_TYPE type       = CObjectManager::GetInst()->GetCurrentMapType();
	const Vec3     startPoint = CObjectManager::GetInst()->GetStartingPoint(type);

	CGameObject* pPlayer = CObjectManager::GetInst()->GetPlayer();
	if (pPlayer)
	{
		pPlayer->Transform()->SetRelativePos(startPoint);
		pPlayer->Transform()->M
		pPlayer->NaviAgent()->lateupdate();
		*/
}

void ObjMgrState_IDLE::LateUpdate()
{
	CGameObject* pDoor = CObjectManager::GetInst()->GetGate()->GetChild(L"DOOR");
	if (CObjectManager::GetInst()->CheckAllMonsterDead() && false == pDoor->GetScript<DoorScript>()->IsDoorOpened())
	{
		pDoor->GetScript<DoorScript>()->SetOpen(true);
		pDoor->Collider3D()->Activate();
	}
}


void ObjMgrState_IDLE::Exit()
{
	const MAP_TYPE type = CObjectManager::GetInst()->GetCurrentMapType();
	if (type == MAP_TYPE::_01)
	{
		wstring soundpath = L"sound\\map01_sound.wav";
		CSound* pSound    = nullptr;
		pSound            = CResMgr::GetInst()->Load<CSound>(soundpath, soundpath).Get();
		pSound->Stop();
	}
}
