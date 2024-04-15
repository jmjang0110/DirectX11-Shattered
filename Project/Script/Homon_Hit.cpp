#include "pch.h"
#include "Homon_Hit.h"

#include "FieldMonsteScript.h"

// engine
#include <Engine/CGameObject.h>
#include <Engine/CAnimator3D.h>

Homon_Hit::Homon_Hit()
	: CState{L"HIT"}
	, m_pAnimation(nullptr)
	, m_pOwnerMGR(nullptr) {}

Homon_Hit::~Homon_Hit() {}

void Homon_Hit::Enter()
{
	CState::ResetTimer();

	if (nullptr == m_pAnimation)
	{
		map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();
		if (mapAnim.find(L"Homon_Hit") == mapAnim.end())
			assert(nullptr && L"Error! Homonculus Anim data is not loaded.");
		m_pAnimation = mapAnim.find(L"Homon_Hit")->second;

		m_pOwnerMGR = (GetOwner()->GetScript<FieldMonsteScript>()->GetMonsterMGR());
	}
	GetOwner()->Animator3D()->Play(m_pAnimation->GetName(), false);

}

void Homon_Hit::Update()
{
	CState::Update();

	if (nullptr == m_pAnimation)
		return;

	if (m_pOwnerMGR->IsRunTimeEXIST())
	{
		if (m_pOwnerMGR->GetRunTime() <= CState::GetTimer())
		{
			GetOwner()->GetScript<FieldMonsteScript>()->SetCurAnimationDone();
			GetOwner()->GetScript<FieldMonsteScript>()->SetHitStateOn(false);
		}
	}
	else
	{
		float fAnimationLength = (float)(m_pAnimation->GetEndTime() - m_pAnimation->GetStartTime());
		if (fAnimationLength <= CState::GetTimer())
		{
			GetOwner()->GetScript<FieldMonsteScript>()->SetCurAnimationDone();
			GetOwner()->GetScript<FieldMonsteScript>()->SetHitStateOn(false);
		}
	}
}

void Homon_Hit::Exit()
{
	m_pAnimation->Reset();
}
