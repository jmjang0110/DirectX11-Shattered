#include "pch.h"
#include "CStateMgr.h"

// [ EMGINE PART ] 
#include <Engine/CFSM.h>


// [ SCRIPT PART ]
#include "CPlayerDieState.h"			// DIE 
#include "CPlayerHeavyAttackState.h"	// HEAVY_ATTACK
#include "CPlayerHitState.h"			// HIT
#include "CPlayerIdleState.h"			// IDLE
#include "CPlayerJumpState.h"			// JUMP	
#include "CPlayerLightAttackState.h"	// LIGHT_ATTACK
#include "CPlayerMoveState.h"			// MOVE
#include "CPlayerSprintState.h"			// SPRINT
#include "CPlayerParryState.h"			// PARRY 


#include "CScriptMgr.h"
#include "PlayerScript.h"
#include "CPlayerMoveState.h"
#include "PlayerCamScript.h"



CStateMgr::CStateMgr()
	: m_pOwnerObj(nullptr)
	, m_sPrevState(L"")
	, m_sCurstate(L"")
	, m_vCamLock_TargetPos(Vec3(0.f, 0.f, 0.f))
	, m_pCamLock_TargetObj(nullptr)


{
}

CStateMgr::~CStateMgr()
{
}

void CStateMgr::Update()
{
	m_sPrevState = m_sCurstate;
	wstring sNewState = ChangeStateByKeyInfo();
	if (sNewState != L"")
		m_sCurstate = sNewState;

	// 현재 상태와 다음 상태가 동일하다면 바꾸지 않는다. 
	if (m_sPrevState == m_sCurstate)
		return;

	// 키 입력에 따른 ChangeState 
	ChangeState(m_sCurstate);

}

// 외부에서 스테이트가 변경되었을 때 StateMgr에 변경점을 알리는 함수 
void CStateMgr::ChangeCurStateType(wstring _StateTypeName)
{
	if (m_sCurstate == _StateTypeName)
		return;

	m_sPrevState = m_sCurstate;
	m_sCurstate = _StateTypeName;

}

void CStateMgr::LateUpdate()
{

}

void CStateMgr::InitState(CGameObject* _pOwner)
{
	m_pOwnerObj = _pOwner;
	CFSM* pFSM = m_pOwnerObj->FSM();
	if (pFSM == nullptr)
		return;

	pFSM->AddState(L"DIE", new CPlayerDieState);
	pFSM->AddState(L"HEAVY_ATTACK", new CPlayerHeavyAttackState);
	pFSM->AddState(L"HIT", new CPlayerHitState);
	pFSM->AddState(L"IDLE", new CPlayerIdleState);
	pFSM->AddState(L"JUMP", new CPlayerJumpState);
	pFSM->AddState(L"LIGHT_ATTACK", new CPlayerLightAttackState);
	pFSM->AddState(L"MOVE", new CPlayerMoveState);
	pFSM->AddState(L"SPRINT", new CPlayerSprintState);
	pFSM->AddState(L"PARRY", new CPlayerParryState);

	pFSM->ChangeState(L"IDLE");
}



void CStateMgr::SetKeyInfo(KEY_STATE _eKeyState, KEY _eKey, tKeyOptionFlags_Zip _tFlags)
{
	m_tCurKeyInfo.eKeyState = _eKeyState;
	m_tCurKeyInfo.eKey = _eKey;
	m_tCurKeyInfo.tKeyFlags_Zip = _tFlags;

}

void CStateMgr::SetCameraLock(bool _b)
{
	// 카메라 잠금 모드 설정 
	if (_b == true)
	{
		m_bCameraLock = _b;
		// 플레이어 카메라 스크립트에 카메라 잠금 모드 설정 O
		if (m_pCamera)
		{
			PlayerCamScript* pCamScript = (PlayerCamScript*)m_pCamera->GetScriptByName(L"PlayerCamScript");
			if (pCamScript)
			{
				// 카메라 잠금 모드 
				pCamScript->SetCameraOption(CAMERA_OPTION::CAMERA_LOCK);
				// 기준이 될 타겟오브젝트와 초기 위치를 넘긴다. 
				Vec3 vCamLockTargetPos = GetCameraLockTargetPos();
				pCamScript->SetCameraLockTargetObj(m_pCamLock_TargetObj, vCamLockTargetPos);
				pCamScript->Reset_CameraLock();
			}
		}


	}
	else
	{
		m_bCameraLock = _b;
		if (m_pCamera)
		{
			PlayerCamScript* pCamScript = (PlayerCamScript*)m_pCamera->GetScriptByName(L"PlayerCamScript");
			if (pCamScript)
			{
				pCamScript->SetCameraOption(CAMERA_OPTION::DEFAULT);
				pCamScript->Reset_Deafult();

			}
		}
	}

}

// 플레이어와 몬스터 사이의 거리 중 가장 가까운 위치를 구한다. 
Vec3 CStateMgr::GetCameraLockTargetPos()
{
	Vec3 vMonsterPos = Vec3(0.f, 0.f, 0.f);
	CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(L"MONSTER");

	float fMinLength = FBXSDK_FLOAT_MAX;
	Vec3 vMinVector = Vec3(0.f, 0.f, 0.f);
	m_pCamLock_TargetObj = nullptr;


	if (pLayer)
	{
		vector<CGameObject*> vRootObjs = pLayer->GetRootObjects();
		for (int i = 0; i < vRootObjs.size(); ++i)
		{
			if (vRootObjs[i] != nullptr)
			{
				Vec3 vMonsterPos = vRootObjs[i]->Transform()->GetRelativePos();
				Vec3 vPlayerPos = m_pOwnerObj->Transform()->GetRelativePos();
				Vec3 vTargetVector = vMonsterPos - vPlayerPos;
				float vTargetVector_Length = vTargetVector.Length();

				// 가장 거리가 가까운 몬스터 위치를 구한다. 
				if (vTargetVector_Length <= fMinLength)
				{
					fMinLength = vTargetVector_Length;
					vMinVector = vTargetVector;
					m_pCamLock_TargetObj = vRootObjs[i];
				}
			}
		}
	}

	//vMonsterPos = vMinVector.Normalize();
	return vMonsterPos;
}

wstring CStateMgr::ChangeStateByKeyInfo()
{
	wstring StateByKey = L"";

	// 키 변화에 다른 상태 변화 
	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::TAP)
	{
		StateByKey = Check_Tap();
	}

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::PRESSED)
	{
		StateByKey = Check_Pressed();
	}

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away & PLAYER_KEY_OPTION::AWAY)
	{
		StateByKey = Check_Away();
	}

	return StateByKey;
}

void CStateMgr::ChangeState(wstring _sStateName)
{
	if (m_pOwnerObj == nullptr)
		return;

	CFSM* pFSM = m_pOwnerObj->FSM();
	if (pFSM == nullptr)
		return;

	pFSM->ChangeState(_sStateName);


}


wstring CStateMgr::Check_Tap()
{
	wstring StateByKey = L"";

	// [ TAP ]

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::FORWARD ||
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::BACKWARD ||
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::LEFT ||
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::RIGHT)
	{
		// [ 점프 ]상태일 땐 MOVE 로 변환하지 않는다.
		if (m_sCurstate != L"JUMP" && m_sCurstate != L"SPRINT")
			StateByKey = L"MOVE";

	}

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::JUMP)
	{
		// [ 스프린트 ] 상태일 땐 JUMP 로 변환하지 않는다. 
		if (m_sCurstate != L"SPRINT")
			StateByKey = L"JUMP";
	}

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::SPRINT)
	{
		StateByKey = L"SPRINT";
	}

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::PARRY)
	{
		StateByKey = L"PARRY";
	}

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::LIGHT_ATTACK)
	{
		// [ 강공격, 점프, 스프린트 ]상태일 땐 약공격 로 변환하지 않는다.
		if (m_sCurstate != L"HEAVY_ATTACK" && m_sCurstate != L"JUMP" &&
			m_sCurstate != L"SPRINT")
			StateByKey = L"LIGHT_ATTACK";

	}

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::HEAVY_ATTACK)
	{
		// [ 약공격, 점프, 스프린트 ]상태일 땐 약공격 로 변환하지 않는다.
		if (m_sCurstate != L"LIGHT_ATTACK" && m_sCurstate != L"JUMP" &&
			m_sCurstate != L"SPRINT")
			StateByKey = L"HEAVY_ATTACK";
	}

	// [ Camera Lock 모드 설정 ]
	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::CAMERA_LOCK)
	{
		m_bCameraLock = !m_bCameraLock;
		SetCameraLock(m_bCameraLock);

	}

	return StateByKey;

}

wstring CStateMgr::Check_Pressed()
{
	wstring StateByKey = L"";

	// [ PRESSED ]

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::FORWARD ||
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::BACKWARD ||
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::LEFT ||
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::RIGHT)
	{

		// [ 점프, 스프린트, 약 공격, 강 공격 ]상태일 땐 MOVE 로 변환하지 않는다.
		if (m_sCurstate != L"JUMP" && m_sCurstate != L"SPRINT" &&
			m_sCurstate != L"LIGHT_ATTACK" && m_sCurstate != L"HEAVY_ATTACK")
			StateByKey = L"MOVE";

		// 방향키를 누르고 있다가 [ 점프 ]를 누른 경우 
		if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::JUMP)
			StateByKey = L"JUMP";

		// 방향키를 누르고 있다가 [ 스프린트 ]를 누른 경우 
		if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::SPRINT)
			StateByKey = L"SPRINT";

		// 방향키를 누르고 있다가 [ 약 공격 ]을 누른 경우 + [ 점프, 스프린트 ] 상태가 아닌 경우  
		if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::LIGHT_ATTACK &&
			m_sCurstate != L"JUMP" && m_sCurstate != L"SPRINT")
			StateByKey = L"LIGHT_ATTACK";

		// 방향키를 누르고 있다가 [ 강 공격 ]을 누른 경우 + [ 점프, 스프린트 ] 상태가 아닌 경우  
		if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::HEAVY_ATTACK &&
			m_sCurstate != L"JUMP" && m_sCurstate != L"SPRINT")
			StateByKey = L"HEAVY_ATTACK";
	}

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::JUMP)
	{

	}

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::SPRINT)
	{


	}

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::PARRY)
	{

	}

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::LIGHT_ATTACK)
	{

	}


	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::HEAVY_ATTACK)
	{

	}

	return StateByKey;

}

wstring CStateMgr::Check_Away()
{
	wstring StateByKey = L"";

	// [ AWAY ]

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away & PLAYER_KEY_OPTION::FORWARD ||
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away & PLAYER_KEY_OPTION::BACKWARD ||
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away & PLAYER_KEY_OPTION::LEFT ||
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away & PLAYER_KEY_OPTION::RIGHT)
	{
		// [ 점프 , 스프린트 ]가 아닌 상태에서 방향키를 누르다가 뗸 경우 
		if (m_sCurstate != L"JUMP" && m_sCurstate != L"SPRINT")
			StateByKey = L"IDLE";


	}

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away & PLAYER_KEY_OPTION::JUMP)
	{

	}

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away & PLAYER_KEY_OPTION::SPRINT)
	{


	}

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away & PLAYER_KEY_OPTION::PARRY)
	{

	}

	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away & PLAYER_KEY_OPTION::LIGHT_ATTACK)
	{

	}


	if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away & PLAYER_KEY_OPTION::HEAVY_ATTACK)
	{

	}

	return StateByKey;

}