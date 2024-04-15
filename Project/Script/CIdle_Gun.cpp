#include "pch.h"
#include "CIdle_Gun.h"

#include <Engine/CAnimator3D.h>
#include <Engine/CGameObject.h>
#include <Engine/CFSM.h>

#include "MonsterGunScript.h"

CIdle_Gun::CIdle_Gun(MonsterGunScript* pScript)
	: CState{L"IDLE"}
	, m_pScript{pScript}
{
}

CIdle_Gun::~CIdle_Gun()
{
}

void CIdle_Gun::Update()
{
}

void CIdle_Gun::Enter()
{
	GetOwner()->Animator3D()->Play(L"IDLE", true);
}

void CIdle_Gun::Exit()
{
	GetOwner()->Animator3D()->GetCurAnim()->Reset();
}
