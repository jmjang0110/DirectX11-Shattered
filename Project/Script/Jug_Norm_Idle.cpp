#include "pch.h"
#include "Jug_Norm_Idle.h"

// [ ENGINE PART ]
#include <Engine/CAnimator3D.h>


Jug_Norm_Idle::Jug_Norm_Idle()
	: CState(L"JUG_NORM_IDLE")
	, m_pAnim(nullptr)
{
}

Jug_Norm_Idle::Jug_Norm_Idle(const Jug_Norm_Idle& _origin)
	: CState(_origin.m_sStateType)
	, m_pAnim(_origin.m_pAnim)
{
}

Jug_Norm_Idle::~Jug_Norm_Idle()
{
}


void Jug_Norm_Idle::Init()
{
	if (nullptr == m_pAnim)
	{
		map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();

		if (mapAnim.find(L"JUG_NORM_IDLE") == mapAnim.end())
			assert(nullptr && L"애니메이션 클립을 찾을 수 없습니다. \n Monster State Script error");

		m_pAnim = mapAnim.find(L"JUG_NORM_IDLE")->second;
	}
}

void Jug_Norm_Idle::Enter()
{
	// IDLE 애니메이션 플레이
	GetOwner()->Animator3D()->Play(L"JUG_NORM_IDLE", true);
}

void Jug_Norm_Idle::Exit()
{
}

void Jug_Norm_Idle::Update()
{
	CState::Update();
}

void Jug_Norm_Idle::LateUpdate()
{
	CState::LateUpdate();
}
