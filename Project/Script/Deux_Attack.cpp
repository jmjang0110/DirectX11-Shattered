#include "pch.h"
#include "Deux_Attack.h"

#include "FieldMonsteScript.h"
#include "CObjectManager.h"
#include "M_AttackScript.h"

// engine
#include <Engine/CGameObject.h>
#include <Engine/CAnimator3D.h>

Deux_Attack::Deux_Attack()
	: CState{L"ATTACK"}
	, m_pAnimation(nullptr)
	, m_pOwnerMGR(nullptr) {}

Deux_Attack::~Deux_Attack() {}

void Deux_Attack::Enter()
{
	CState::ResetTimer();

	if (nullptr == m_pAnimation)
	{
		map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();
		if (mapAnim.find(L"Deux_Attack") == mapAnim.end())
			assert(nullptr && L"Error! Deuxiemie Anim data is not loaded.");
		m_pAnimation = mapAnim.find(L"Deux_Attack")->second;

		m_pOwnerMGR = (GetOwner()->GetScript<FieldMonsteScript>()->GetMonsterMGR());
	}
	m_pAnimation->SetLerpTime(1.5f);
	GetOwner()->Animator3D()->Play(m_pAnimation->GetName(), false);

	Vec3 playerPos  = CObjectManager::GetInst()->GetPlayer()->Transform()->GetRelativePos();
	Vec3 MonsterPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 halfPos    = (playerPos - MonsterPos) / 2.f;

	CObjectManager::GetInst()->CreateAttackCollider(0.5f, 200.f, MonsterPos + halfPos);

	CResMgr::GetInst()->FindRes<CSound>(L"sound\\Deux\\LegioAttackVoice1.wav")->Play(1, 0.5f, false);
}

void Deux_Attack::Update()
{
	CState::Update();

	if (nullptr == m_pAnimation)
		return;

	// 특정 프레임에 아래 함수호출. 위치조정 필요
	// GetOwner()->AddComponent(new M_AttackScript);
	//	CObjectManager::GetInst()->CreateAttackCollider(1.f, 100.f, GetOwner()->Transform()->GetRelativePos());


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

void Deux_Attack::Exit()
{
	m_pAnimation->Reset();
}
