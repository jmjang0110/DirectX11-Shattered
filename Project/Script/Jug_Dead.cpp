#include "pch.h"
#include "Jug_Dead.h"

// [ ENGINE PART ]
#include <Engine/CAnimator3D.h>
#include <Engine/CTransform.h>
#include <Engine/CTimeMgr.h>

Jug_Dead::Jug_Dead()
	: CState(L"JUG_DEAD")
	, m_pAnim(nullptr)
{
}

Jug_Dead::Jug_Dead(const Jug_Dead& _origin)
	: CState(_origin.m_sStateType)
{
}

Jug_Dead::~Jug_Dead()
{
}

void Jug_Dead::Init()
{
	if (nullptr == m_pAnim)
	{
		map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();

		if (mapAnim.find(L"JUG_DEAD") == mapAnim.end())
			assert(nullptr && L"애니메이션 클립을 찾을 수 없습니다. \n Boss State Script error");

		m_pAnim = mapAnim.find(L"JUG_DEAD")->second;
	}
}

void Jug_Dead::Enter()
{
	// IDLE 애니메이션 플레이
	GetOwner()->Animator3D()->Play(L"JUG_DEAD", false);
}

void Jug_Dead::Exit()
{
}

void Jug_Dead::Update()
{
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	if (vPos.y > -210.f)
	{
		vPos.y -= DT * 100.f;

		GetOwner()->Transform()->SetRelativePos(vPos);
	}
}

void Jug_Dead::LateUpdate()
{
	CState::LateUpdate();
}

