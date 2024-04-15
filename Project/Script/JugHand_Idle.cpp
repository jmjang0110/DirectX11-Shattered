#include "pch.h"
#include "JugHand_Idle.h"

#include "BossJugHandScript.h"

// Engine
#include <Engine/CAnimation3D.h>
#include <Engine/CAnimator3D.h>

JugHand_Idle::JugHand_Idle()
	: CState{L"IDLE"}
	, m_pAnimation(nullptr)
	, m_fLerfTime(0)
{
}

JugHand_Idle::~JugHand_Idle()
{
}

void JugHand_Idle::Enter()
{
	CState::ResetTimer();

	CScript* pScript = GetOwner()->GetScript<BossJugHandScript>();
	int iIndex = ((BossJugHandScript*)pScript)->GetHandIndexNumber();
	wstring sAnimName = L"";

	if (nullptr == m_pAnimation)
	{
		map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();

		if (1 == iIndex)
		{
			sAnimName = L"Hand01_Idle";
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
			assert(nullptr && L"애니메이션 클립을 찾을 수 없습니다. \n Hand State Script error");

		m_pAnimation = mapAnim.find(sAnimName)->second;
	}
	sAnimName = m_pAnimation->GetName();
	GetOwner()->Animator3D()->Play(sAnimName, true);

}

void JugHand_Idle::Update()
{
	CState::Update();

	// [ Idle routine ]
	// 1. 상태가 바뀌지 않는다면 계속 Idle 유지.
	// 2. 루틴 종료

	CScript* pScript = pScript = GetOwner()->GetScript<BossJugHandScript>();

	if (nullptr == pScript)
		return;
}

void JugHand_Idle::Exit()
{
}
