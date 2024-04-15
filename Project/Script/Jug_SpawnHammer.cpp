#include "pch.h"
#include "Jug_SpawnHammer.h"

// [ ENGINE PART ]
#include <Engine/CAnimator3D.h>

Jug_SpawnHammer::Jug_SpawnHammer()
	: CState(L"JUG_SPAWNHAMMER")
	, m_pAnim(nullptr)
{
}

Jug_SpawnHammer::Jug_SpawnHammer(const Jug_SpawnHammer& _origin)
	: CState(_origin.m_sStateType)
	, m_pAnim(_origin.m_pAnim)
{
}

Jug_SpawnHammer::~Jug_SpawnHammer()
{
}

void Jug_SpawnHammer::Init()
{
	if (nullptr == m_pAnim)
	{
		map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();

		if (mapAnim.find(L"JUG_SPAWNHAMMER") == mapAnim.end())
			assert(nullptr && L"애니메이션 클립을 찾을 수 없습니다. \n Boss State Script error");

		m_pAnim = mapAnim.find(L"JUG_SPAWNHAMMER")->second;
	}
}

void Jug_SpawnHammer::Enter()
{
	// IDLE 애니메이션 플레이
	GetOwner()->Animator3D()->Play(L"JUG_SPAWNHAMMER", false);
}

void Jug_SpawnHammer::Exit()
{
}

void Jug_SpawnHammer::Update()
{
}

void Jug_SpawnHammer::LateUpdate()
{
	CState::LateUpdate();
}
