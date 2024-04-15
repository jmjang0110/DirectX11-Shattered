#include "pch.h"
#include "CPlayerIdleState.h"

#include <Engine/CAnimator3D.h>
#include <Engine/CFSM.h>
#include "PlayerScript.h"

#include "CStateMgr.h"
#include "TrailScript.h"
#include "PaperBurnScript.h"


CPlayerIdleState::CPlayerIdleState()
	: CState(L"IDLE")
	, m_pStateMgr(nullptr)

{
}

CPlayerIdleState::CPlayerIdleState(const CPlayerIdleState& _origin)
	:CState(_origin)
{
}

CPlayerIdleState::~CPlayerIdleState()
{
}


void CPlayerIdleState::Enter()
{
	CState::ResetTimer();
	PlayIdleAnim(L"idle", true);

	// StateMGr 에 현재상태가 변경됨을 알린다. 
	PlayerScript* pScript = (PlayerScript*)GetOwner()->GetScriptByName(L"PlayerScript");
	pScript->GetSwordBoneCollider()->Deactivate();

	if (pScript != nullptr)
	{
		m_pStateMgr = pScript->GetStateMgr();
		m_pStateMgr->ChangeCurStateType(L"IDLE");
	}

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

void CPlayerIdleState::Exit()
{

}

void CPlayerIdleState::Update()
{
	CState::Update();


}

void CPlayerIdleState::LateUpdate()
{
	CState::LateUpdate();


}


void CPlayerIdleState::PlayIdleAnim(wstring _sName, bool _bRepeat)
{
	CAnimator3D* pAnimator3D = GetOwner()->Animator3D();
	if (pAnimator3D != nullptr)
	{
		if (pAnimator3D->FindAnim(_sName) != nullptr)
			pAnimator3D->Play(_sName, _bRepeat);
	}
}