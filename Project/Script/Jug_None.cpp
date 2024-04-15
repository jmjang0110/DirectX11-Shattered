#include "pch.h"
#include "Jug_None.h"

// [ ENGINE PART ]
#include <Engine/CAnimator3D.h>


Jug_None::Jug_None()
	: CState(L"JUG_NONE")
	, m_pAnim(nullptr)
{

}

Jug_None::Jug_None(const Jug_None& _origin)
	: CState(_origin.m_sStateType)
	, m_pAnim(nullptr)
{
}

Jug_None::~Jug_None()
{
}

void Jug_None::Enter()
{
	// 현재 애니메이션이 있는 경우 초기화한다
	//if(nullptr != GetOwner()->Animator3D() && nullptr != GetOwner()->Animator3D()->GetCurAnim())
	//{
	//	// 리셋
	//	wstring curAnim = GetOwner()->Animator3D()->GetCurAnim()->GetName();
	//	GetOwner()->Animator3D()->SetAnimState(curAnim, ANIMATION_STATE::BEFORE_PLAY);
	//}
}

void Jug_None::Exit()
{
}

void Jug_None::Update()
{
	CState::Update();
}

void Jug_None::LateUpdate()
{
	CState::LateUpdate();
}
