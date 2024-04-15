#include "pch.h"
#include "CPlayerHitState.h"
#include <Engine/CAnimator3D.h>
#include <Engine/CFSM.h>
#include "PaperBurnScript.h"
#include "CStateMgr.h"
#include "PlayerScript.h"


CPlayerHitState::CPlayerHitState()
	: CState(L"HIT")
{

}

CPlayerHitState::CPlayerHitState(const CPlayerHitState& _origin)
	:CState(_origin)
{

}

CPlayerHitState::~CPlayerHitState()
{

}


void CPlayerHitState::Enter()
{
	CState::ResetTimer();
	m_eState = HIT_STATE::HIT_SUCCESS;
	PlayHitAnim(L"hit", false, 0.5f);
	if (m_pPaperBurn == nullptr)
	{
		m_pPaperBurn = GetOwner()->GetScript<PaperBurnScript>();

	}

	if (m_pPaperBurn)
	{
		m_pPaperBurn->On();
		m_pPaperBurn->SetDir(1);

	}

	PlayerScript* pScript = (PlayerScript*)GetOwner()->GetScriptByName(L"PlayerScript");
	pScript->GetStateMgr()->ChangeCurStateType(L"HIT");

}

void CPlayerHitState::Exit()
{
	if (m_pPaperBurn)
	{
		m_pPaperBurn->Off();
		m_pPaperBurn->SetDir(1);

	}
}

void CPlayerHitState::Update()
{
	CState::Update();

	switch (m_eState)
	{
	case HIT_STATE::HIT_SUCCESS:
	{
		if (m_pCurAnim != nullptr)
		{
			double animTime = m_pCurAnim->GetEndTime() - m_pCurAnim->GetStartTime();
			double CurTime = m_pCurAnim->GetAccTime();


			if (CurTime >= animTime * 0.5) // 애니메이션 반이 지났을 때 
			{
				if (m_pPaperBurn)
				{
					//m_pPaperBurn->Stop();
					m_pPaperBurn->SetDir(-1);
				}
			}
		}

		if (m_pCurAnim != nullptr && m_pCurAnim->IsFinish())
		{
			GetOwner()->FSM()->ChangeState(L"IDLE");
			if (m_pPaperBurn)
			{
				m_pPaperBurn->Off();
				m_pPaperBurn->SetDir(1);
			}
		}

	}
	break;
	case HIT_STATE::HIT_FAIL:
	{

	}
	break;
	}

}

void CPlayerHitState::LateUpdate()
{
	CState::LateUpdate();

	switch (m_eState)
	{
	case HIT_STATE::HIT_SUCCESS:
		break;
	case HIT_STATE::HIT_FAIL:
		break;

	}
}

void CPlayerHitState::PlayHitAnim(wstring _sName, bool _bRepeat, float _fSpeed)
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
