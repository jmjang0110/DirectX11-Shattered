#include "pch.h"
#include "Jug_Hammer_Hit.h"

// [ ENGINE PART ]
#include <Engine/CAnimator3D.h>

Jug_Hammer_Hit::Jug_Hammer_Hit()
	: CState(L"JUG_HAMMER_HIT")
	, m_pAnim(nullptr)
{
}

Jug_Hammer_Hit::Jug_Hammer_Hit(const Jug_Hammer_Hit& _origin)
	: CState(_origin.m_sStateType)
	, m_pAnim(_origin.m_pAnim)
{
}

Jug_Hammer_Hit::~Jug_Hammer_Hit()
{
}

void Jug_Hammer_Hit::Init()
{
	if (nullptr == m_pAnim)
	{
		map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();

		if (mapAnim.find(L"JUG_HAMMER_HIT") == mapAnim.end())
			assert(nullptr && L"애니메이션 클립을 찾을 수 없습니다. \n Boss State Script error");

		m_pAnim = mapAnim.find(L"JUG_HAMMER_HIT")->second;
	}
}

void Jug_Hammer_Hit::Enter()
{
	// IDLE 애니메이션 플레이
	GetOwner()->Animator3D()->Play(L"JUG_HAMMER_HIT", true);
}

void Jug_Hammer_Hit::Exit()
{
}

void Jug_Hammer_Hit::Update()
{
	CState::Update();
}

void Jug_Hammer_Hit::LateUpdate()
{
	CState::LateUpdate();
}

