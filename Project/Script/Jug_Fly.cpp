#include "pch.h"
#include "Jug_Fly.h"

// [ ENGINE PART ]
#include <Engine/CAnimator3D.h>

Jug_Fly::Jug_Fly()
	: CState(L"JUG_FLY")
	, m_pAnim(nullptr)
{
}

Jug_Fly::Jug_Fly(const Jug_Fly& _origin)
	: CState(_origin.m_sStateType)
	, m_pAnim(_origin.m_pAnim)
{
}

Jug_Fly::~Jug_Fly()
{
}

void Jug_Fly::Init()
{
	if (nullptr == m_pAnim)
	{
		map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();

		if (mapAnim.find(L"JUG_FLY") == mapAnim.end())
			assert(nullptr && L"애니메이션 클립을 찾을 수 없습니다. \n Boss State Script error");

		m_pAnim = mapAnim.find(L"JUG_FLY")->second;
	}
}

void Jug_Fly::Enter()
{
	// IDLE 애니메이션 플레이
	GetOwner()->Animator3D()->Play(L"JUG_FLY", true);
}

void Jug_Fly::Exit()
{
}

void Jug_Fly::Update()
{
	CState::Update();
}

void Jug_Fly::LateUpdate()
{
	CState::LateUpdate();
}
