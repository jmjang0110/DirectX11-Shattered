#include "pch.h"
#include "Deux_Idle.h"

#include "FieldMonsteScript.h"

// engine
#include <Engine/CGameObject.h>
#include <Engine/CAnimator3D.h>

Deux_Idle::Deux_Idle()
	: CState{L"IDLE"}
	, m_pAnimation(nullptr)
	, m_pOwnerMGR(nullptr) {}

Deux_Idle::~Deux_Idle() {}

void Deux_Idle::Enter()
{
	CState::ResetTimer();

	if (nullptr == m_pAnimation)
	{
		map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();
		if (mapAnim.find(L"Deux_Idle") == mapAnim.end())
			assert(nullptr && L"Error! Deuxiemie Anim data is not loaded.");
		m_pAnimation = mapAnim.find(L"Deux_Idle")->second;

		m_pOwnerMGR = (GetOwner()->GetScript<FieldMonsteScript>()->GetMonsterMGR());
	}
	m_pAnimation->SetLerpTime(1.f);
	GetOwner()->Animator3D()->Play(m_pAnimation->GetName(), true);
}

void Deux_Idle::Update()
{
	CState::Update();


	if (nullptr == m_pAnimation)
		return;

	if (m_pOwnerMGR->IsRunTimeEXIST())
	{
		if (m_pOwnerMGR->GetRunTime() <= CState::GetTimer())
		{
			GetOwner()->GetScript<FieldMonsteScript>()->SetCurAnimationDone();
		}
	}
	else
	{
		float fAnimationLength = (float)(m_pAnimation->GetEndTime() - m_pAnimation->GetStartTime());
		if (fAnimationLength <= CState::GetTimer())
		{
			GetOwner()->GetScript<FieldMonsteScript>()->SetCurAnimationDone();
		}
	}
}

void Deux_Idle::Exit()
{
	m_pAnimation->Reset();
}
