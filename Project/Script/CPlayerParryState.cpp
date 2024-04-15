#include "pch.h"
#include "CPlayerParryState.h"

#include "PlayerScript.h"
#include "TrailScript.h"
#include "CStateMgr.h"
#include"PaperBurnScript.h"


#include <Engine/CFSM.h>
#include <Engine/Ccamera.h>
#include <Engine/CGameObject.h>
#include <Engine/CTransform.h>
#include <Engine/CTimeMgr.h>
#include <Engine/CAnimator3D.h>
CPlayerParryState::CPlayerParryState()
	: CState(L"PARRY")
{
}

CPlayerParryState::CPlayerParryState(const CPlayerParryState& _origin)
	: CState(_origin)

{
}

CPlayerParryState::~CPlayerParryState()
{
}


void CPlayerParryState::Enter()
{
	CState::ResetTimer();
	// [ 스크립트, 상태 매니저 ] 초기화
	m_eParryState = PARRY_STATE::TRY;
	if (m_pPlayerScript == nullptr)
	{
		m_pPlayerScript = (PlayerScript*)GetOwner()->GetScriptByName(L"PlayerScript");
		m_pStateMgr = m_pPlayerScript->GetStateMgr();

		// StateMGr 에 현재상태가 변경됨을 알린다. 
		m_pStateMgr = m_pPlayerScript->GetStateMgr();
		m_pStateMgr->ChangeCurStateType(L"PARRY");
	}


	// [ 애니메이션 ]플레이 
	PlayParryAnim(L"parry_1", false, 0.3f);

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

void CPlayerParryState::Exit()
{

}

void CPlayerParryState::Update()
{
	CState::Update();

	// 패리 [상태/애니메이션] 업데이트 
	UpdateParryState();

}

void CPlayerParryState::LateUpdate()
{
	CState::LateUpdate();

}




void CPlayerParryState::UpdateParryState()
{
	switch (m_eParryState)
	{

	case PARRY_STATE::TRY:
	{
		// 애니메이션이 끝났다면 
		if (m_pCurAnim != nullptr && m_pCurAnim->IsFinish())
		{
			// [ 패리 성공 ]
			if (m_bParrySuccess)
			{
				m_eParryState = PARRY_STATE::SUCCESS;
				PlayParryAnim(L"parry_2", false, 0.2f);
			}
			// [ 패리 실패 ]
			else
			{
				m_eParryState = PARRY_STATE::FAIL;
				PlayParryAnim(L"parry_3", false, 0.8f);
			}
		}
	}
	break;
	case PARRY_STATE::FAIL:
	{
		// parry_3 애니메이션이 끝났다면 
		if (m_pCurAnim != nullptr && m_pCurAnim->IsFinish())
		{
			GetOwner()->FSM()->ChangeState(L"IDLE");
		}
	}
	break;
	case PARRY_STATE::SUCCESS:
	{
		// parry_2 애니메이션이 끝났다면 
		if (m_pCurAnim != nullptr && m_pCurAnim->IsFinish())
		{
			m_eParryState = PARRY_STATE::END;
			PlayParryAnim(L"parry_3", false, 0.8f);
		}
	}
	case PARRY_STATE::END:
	{

		// PARRY -> IDLE
		if (m_pCurAnim != nullptr && m_pCurAnim->IsFinish())
		{
			GetOwner()->FSM()->ChangeState(L"IDLE");
		}
	}
	break;

	}
}

void CPlayerParryState::PlayParryAnim(wstring _sName, bool _bRepeat, float _fSpeed = 1.f)
{
	CAnimator3D* pAnimator3D = GetOwner()->Animator3D();
	if (pAnimator3D != nullptr)
	{
		CAnimation3D* pAnim = pAnimator3D->FindAnim(_sName);
		m_pCurAnim = pAnim;
		if (pAnim != nullptr)
		{
			pAnimator3D->Play(_sName, _bRepeat);
			pAnimator3D->SetSpeed(_fSpeed);

		}
	}

}
