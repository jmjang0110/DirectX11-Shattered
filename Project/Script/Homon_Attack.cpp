#include "pch.h"
#include "Homon_Attack.h"

#include "FieldMonsteScript.h"
#include "FieldM_StateMgr.h"
#include "CObjectManager.h"
#include "M_AttackScript.h"

// engine
#include <Engine/CGameObject.h>
#include <Engine/CAnimator3D.h>
#include <Engine/CCollider3D.h>

Homon_Attack::Homon_Attack()
	: CState{L"ATTACK"}
	, m_pAnimation(nullptr)
	, m_pOwnerMGR(nullptr) {}

Homon_Attack::~Homon_Attack() {}

void Homon_Attack::Enter()
{
	CState::ResetTimer();

	if (nullptr == m_pAnimation)
	{
		map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();
		if (mapAnim.find(L"Homon_Attack2") == mapAnim.end())
			assert(nullptr && L"Error! Homonculus Anim data is not loaded.");
		m_pAnimation = mapAnim.find(L"Homon_Attack2")->second;

		m_pOwnerMGR = (GetOwner()->GetScript<FieldMonsteScript>()->GetMonsterMGR());
	}
	GetOwner()->Animator3D()->Play(m_pAnimation->GetName(), false);

	CObjectManager::GetInst()->CreateAttackCollider(0.5f, 300.f, GetOwner()->Transform()->GetRelativePos());

	CResMgr::GetInst()->FindRes<CSound>(L"sound\\Homon\\attack.wav")->Play(1, 0.5f, false);
}


void Homon_Attack::Update()
{
	CState::Update();

	if (nullptr == m_pAnimation)
		return;

	// 특정 프레임에 아래 함수호출. 위치조정 필요
	// GetOwner()->AddComponent(new M_AttackScript);
	//CObjectManager::GetInst()->CreateAttackCollider(1.f, 100.f, GetOwner()->Transform()->GetRelativePos());

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

void Homon_Attack::Exit()
{
	m_pAnimation->Reset();
}
