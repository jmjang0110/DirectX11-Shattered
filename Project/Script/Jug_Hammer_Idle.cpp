#include "pch.h"
#include "Jug_Hammer_Idle.h"

// [ ENGINE PART ]
#include <Engine/CAnimator3D.h>

Jug_Hammer_Idle::Jug_Hammer_Idle()
	: CState(L"JUG_HAMMER_IDLE")
	, m_pAnim(nullptr)
{
}

Jug_Hammer_Idle::Jug_Hammer_Idle(const Jug_Hammer_Idle& _origin)
	: CState(_origin.m_sStateType)
{
}

Jug_Hammer_Idle::~Jug_Hammer_Idle()
{
}

void Jug_Hammer_Idle::Init()
{
	if (nullptr == m_pAnim)
	{
		map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();

		if (mapAnim.find(L"JUG_HAMMER_IDLE") == mapAnim.end())
			assert(nullptr && L"애니메이션 클립을 찾을 수 없습니다. \n Monster State Script error");

		m_pAnim = mapAnim.find(L"JUG_HAMMER_IDLE")->second;
	}
}

void Jug_Hammer_Idle::Enter()
{
	// IDLE 애니메이션 플레이
	GetOwner()->Animator3D()->Play(L"JUG_HAMMER_IDLE", true);
}

void Jug_Hammer_Idle::Exit()
{
}

void Jug_Hammer_Idle::Update()
{
	CState::Update();
}

void Jug_Hammer_Idle::LateUpdate()
{
	CState::LateUpdate();
}

