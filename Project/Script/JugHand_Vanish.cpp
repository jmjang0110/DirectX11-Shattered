#include "pch.h"
#include "JugHand_Vanish.h"

#include "BossJugHandScript.h"
#include "PaperBurnScript.h"

// Engine
#include <Engine/CAnimation3D.h>
#include <Engine/CAnimator3D.h>

JugHand_Vanish::JugHand_Vanish()
	: CState{L"VANISH"}
	, m_pAnimation(nullptr)
	, m_fLerfTime(0.f)
	, m_bPaperburnOn(false)
{
}

JugHand_Vanish::~JugHand_Vanish()
{
}

void JugHand_Vanish::Enter()
{
	CState::ResetTimer();

	m_fLerfTime = 2.f;

	CScript* pScript = GetOwner()->GetScript<BossJugHandScript>();
	int      iIndex = ((BossJugHandScript*)pScript)->GetHandIndexNumber();
	wstring  sAnimName = L"";

	if (nullptr == m_pAnimation)
	{
		map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();

		if (1 == iIndex)
		{
			sAnimName = L"Hand01_Smashing";
		}
		else if (2 == iIndex)
		{
			sAnimName = L"Hand02_Idle";
		}
		else
		{
			sAnimName = L"Hand03_Idle";
		}

		if (mapAnim.find(sAnimName) == mapAnim.end())
			assert(nullptr && L"Hand Vanish Animation is not exist!");

		m_pAnimation = mapAnim.find(sAnimName)->second;
	}

	sAnimName = m_pAnimation->GetName();
	// Vanish는 공격 모션 마지막 Frame 을 계속 유지한다.


	if (1 == iIndex)
	{
		//GetOwner()->Animator3D()->Play(sAnimName, false);
		m_pAnimation->SetAnimState(ANIMATION_STATE::STOP);
		m_pAnimation->SetCurFrameIdx(m_pAnimation->GetEndFrameIdx());
		((BossJugHandScript*)pScript)->SetRunningTime(3.f);
	}
	else
	{
		m_pAnimation->SetLerpTime(0.8f);
		GetOwner()->Animator3D()->Play(sAnimName, true);
		((BossJugHandScript*)pScript)->SetRunningTime(3.f);
	}


	CScript* pPaperburnScript = GetOwner()->GetScript<PaperBurnScript>();

	((PaperBurnScript*)pPaperburnScript)->On();
	((PaperBurnScript*)pPaperburnScript)->SetDir(1.f);
	((PaperBurnScript*)pPaperburnScript)->SetStrength(0.f);

}

void JugHand_Vanish::Update()
{
	CState::Update();

	CScript* pScript = GetOwner()->GetScript<BossJugHandScript>();
	int      iIndex = ((BossJugHandScript*)pScript)->GetHandIndexNumber();


	if (((BossJugHandScript*)pScript)->GetRunningTime() < CState::GetTimer())
	{
		((BossJugHandScript*)pScript)->SetVanishStateDone(true);
		return;
	}
}

void JugHand_Vanish::Exit()
{

	GetOwner()->Deactivate();
}
