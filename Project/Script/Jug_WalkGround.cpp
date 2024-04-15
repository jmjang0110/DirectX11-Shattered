#include "pch.h"
#include "Jug_WalkGround.h"

// [ ENGINE PART ]
#include <Engine/CAnimator3D.h>

Jug_WalkGround::Jug_WalkGround()
	: CState(L"JUG_WALKGROUND")
	, m_pAnim(nullptr)
{
}

Jug_WalkGround::Jug_WalkGround(const Jug_WalkGround& _origin)
	: CState(_origin.m_sStateType)
{
}

Jug_WalkGround::~Jug_WalkGround()
{
}

void Jug_WalkGround::Init()
{
	if (nullptr == m_pAnim)
	{
		map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();

		if (mapAnim.find(L"JUG_WALKGROUND") == mapAnim.end())
			assert(nullptr && L"애니메이션 클립을 찾을 수 없습니다. \n Boss State Script error");

		m_pAnim = mapAnim.find(L"JUG_WALKGROUND")->second;
	}
}

void Jug_WalkGround::Enter()
{
	// IDLE 애니메이션 플레이
	GetOwner()->Animator3D()->Play(L"JUG_WALKGROUND", true);
}

void Jug_WalkGround::Exit()
{
}

void Jug_WalkGround::Update()
{
	CState::Update();
}

void Jug_WalkGround::LateUpdate()
{
	CState::LateUpdate();
}

