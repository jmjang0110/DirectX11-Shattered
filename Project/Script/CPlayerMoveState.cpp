#include "pch.h"
#include "CPlayerMoveState.h"

#include <Engine/Ccamera.h>
#include <Engine/CGameObject.h>
#include <Engine/CTransform.h>
#include <Engine/CTimeMgr.h>
#include <Engine/CAnimator3D.h>
#include "TrailScript.h"


#include "PlayerScript.h"
#include "CStateMgr.h"

#include "CScriptMgr.h"
#include "CPlayerStat.h"
#include "CObjKeyMgr.h"

#include "PlayerCamScript.h"
#include "PaperBurnScript.h"




CPlayerMoveState::CPlayerMoveState()
	:CState(L"MOVE")
	, m_pTranslateMgr(nullptr)
	, m_pCamScript(nullptr)
	, m_bChangeLerpTime(false)
{

}

CPlayerMoveState::CPlayerMoveState(const CPlayerMoveState& _origin)
	:CState(_origin)
	, m_pTranslateMgr(nullptr)
	, m_pCamScript(nullptr)
	, m_bChangeLerpTime(false)
{
}

CPlayerMoveState::~CPlayerMoveState()
{
	SAFE_DELETE(m_pTranslateMgr);

}


void CPlayerMoveState::Enter()
{
	CState::ResetTimer();

	CScript* pScript = CState::GetOwner()->GetScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT);
	PlayerScript* pPlayerScript = (PlayerScript*)pScript;
	m_bChangeLerpTime = false;

	pPlayerScript->GetSwordBoneCollider()->Deactivate();

	if (m_pTranslateMgr == nullptr)
	{
		CGameObject* pCamera = pPlayerScript->GetCamera();
		m_pTranslateMgr = new CTranslateMgr;
		m_pTranslateMgr->Init(GetOwner(), pCamera, pScript);

	}

	m_eMode = MOVE_MODE::RUN;

	// 애니메이션 재생
	PlayMoveAnim(L"run", true);

	// StateMGr 에 현재상태가 변경됨을 알린다. 
	m_pStateMgr = pPlayerScript->GetStateMgr();
	m_pStateMgr->ChangeCurStateType(L"MOVE");


	TrailScript* pTrail = (TrailScript*)GetOwner()->GetScriptByName(L"TrailScript");
	if (pTrail)
	{
		pTrail->Off();
	}

	PaperBurnScript* pBurnScript = GetOwner()->GetScript<PaperBurnScript>();
	if (pBurnScript)
	{
		pBurnScript->Off();
		pBurnScript->SetDir(1);

	}


}

void CPlayerMoveState::Exit()
{
	// 애니메이션의 보간 시간을 기존 보간시간으로 되돌린다.
	if (m_bChangeLerpTime == true)
	{
		CAnimation3D* pAnim = GetOwner()->Animator3D()->GetCurAnim();
		pAnim->SetLerpTime(m_fStandardLerpTime);
		m_bChangeLerpTime = false;
	}
}



void CPlayerMoveState::Update()
{
	CState::Update();

	// MOVE_MODE 업데이트 
	UpdateMoveMode();

	switch (m_eMode)
	{
	case MOVE_MODE::RUN:
	{
		UpdateRunMode();
	}
	break;
	case MOVE_MODE::WALK:
	{
		UpdateWalkMode();

	}
	break;
	}


	// 이동 업데이트 
	m_pTranslateMgr->Update();

	// 애니메이션 보간 시간 조정 
	UpdateRunAnimLerpTime();
}

void CPlayerMoveState::LateUpdate()
{
	CState::LateUpdate();

	m_pTranslateMgr->LateUpdate();

}

void CPlayerMoveState::UpdateMoveMode()
{
	if (m_pCamScript)
	{
		CAMERA_OPTION eCamOption = m_pCamScript->GetCameraOption();
		switch (eCamOption)
		{
		case CAMERA_OPTION::DEFAULT:
			m_eMode = MOVE_MODE::RUN;
			m_pTranslateMgr->SetEqualize_Player_and_Camera(false);
			m_pStat->SetSpeed(150.f);


			break;
		case CAMERA_OPTION::CAMERA_LOCK:
			m_eMode = MOVE_MODE::WALK;
			m_pTranslateMgr->SetEqualize_Player_and_Camera(true);
			m_pStat->SetSpeed(100.f);

			break;
		}
	}
	else
	{
		PlayerScript* pPlayer = (PlayerScript*)CState::GetOwner()->GetScriptByName(L"PlayerScript");
		m_pStat = pPlayer->GetPlayerStat();
		if (pPlayer)
		{

			CGameObject* pCam = pPlayer->GetCamera();
			m_pCamScript = (PlayerCamScript*)pCam->GetScriptByName(L"PlayerCamScript");

		}
	}

}

void CPlayerMoveState::UpdateRunMode()
{
	CAnimation3D* pCurAnim = CState::GetOwner()->Animator3D()->GetCurAnim();
	if (pCurAnim)
	{
		if (pCurAnim->GetName() != L"run")
		{
			// 애니메이션 재생
			PlayMoveAnim(L"run", true);
		}
	}
}

void CPlayerMoveState::UpdateWalkMode()
{
	tKey_Zip tCurKeyInfo = m_pStateMgr->GetCurKeyInfo();
	CAnimation3D* pCurAnim = CState::GetOwner()->Animator3D()->GetCurAnim();
	wstring sAnimName = L"";

	// 하나씩 눌렀을 때 
	if (tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::FORWARD)
	{
		sAnimName = L"walk_forward";
	}

	if (tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::BACKWARD)
	{
		sAnimName = L"walk_back";
	}

	if (tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::RIGHT)
	{
		sAnimName = L"walk_right";
	}

	if (tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::LEFT)
	{
		sAnimName = L"walk_left";
	}

	// 동시에 눌렀을 때  
	if (tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::RIGHT &&
		tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::LEFT)
		sAnimName = L"walk_forward";

	if (tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::FORWARD &&
		tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed & PLAYER_KEY_OPTION::BACKWARD)
		sAnimName = L"walk_forward";



	// 애니메이션 업데이트 
	if (pCurAnim->GetName() != sAnimName)
		PlayMoveAnim(sAnimName, true);

}




void CPlayerMoveState::SetCurKeyInfo(tKey_Zip _keyInfo)
{
	m_tCurKeyInfo = _keyInfo;
}

void CPlayerMoveState::PlayMoveAnim(wstring _sName, bool _bRepeat)
{
	CAnimator3D* pAnimator3D = GetOwner()->Animator3D();
	if (pAnimator3D != nullptr)
	{
		if (pAnimator3D->FindAnim(_sName) != nullptr)
			pAnimator3D->Play(_sName, _bRepeat);
	}
}

void CPlayerMoveState::UpdateRunAnimLerpTime()
{
	// "Run" Animation이 연속으로 재생될 때 보간시간을 [변경]한다.  
	if (m_bChangeLerpTime == true)
		return;

	CAnimator3D* pAnimator3D = GetOwner()->Animator3D();
	if (pAnimator3D != nullptr)
	{
		CAnimation3D* pAnim = pAnimator3D->GetCurAnim();
		// 해당 애니메이션이 2 번째 플레이 될 때 
		if (pAnim != nullptr && pAnim->GetPlayCnt() == 2)
		{
			// 기존 보간시간 저장 
			m_bChangeLerpTime = true;
			m_fStandardLerpTime = pAnim->GetLerpTime();

			// 보간 시간 변경 
			float fChangeLerpTime = 0.01f;
			pAnimator3D->SetLerpTimeOnAnim(pAnim->GetName(), fChangeLerpTime);
		}

	}
}
