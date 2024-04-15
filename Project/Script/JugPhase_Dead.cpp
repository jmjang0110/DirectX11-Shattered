#include "pch.h"
#include "JugPhase_Dead.h"

#include <Engine/CFSM.h>
#include <Engine/CState.h>
#include <Engine/CAnimator3D.h>

#include "BossJugCombatMgrScript.h"

JugPhase_Dead::JugPhase_Dead()
	: CState(L"JUG_PHASE_DEAD")
	, m_pCombatMgr(nullptr)
	, m_pBossFSM(nullptr)
{
}

JugPhase_Dead::JugPhase_Dead(const JugPhase_Dead& _origin)
	: CState(_origin.m_sStateType)
	, m_pCombatMgr(_origin.m_pCombatMgr)
	, m_pBossFSM(_origin.m_pBossFSM)
{
}

JugPhase_Dead::~JugPhase_Dead()
{
}


void JugPhase_Dead::Init()
{
	m_pCombatMgr = GetOwner()->GetScript<BossJugCombatMgrScript>();
	assert(m_pCombatMgr);

	m_pBossFSM = m_pCombatMgr->GetJug()->FSM();
	assert(m_pBossFSM);
}

void JugPhase_Dead::Enter()
{
	// 현재 보스 상태를 JUG_SPAWNHAMMER로 바꾼다.
	m_pBossFSM->ChangeState(GAME::BOSS::JUG_DEAD);

	CGameObject* pPot = m_pCombatMgr->GetPot();
	pPot->Animator3D()->Play(L"Pot_Fall");


	CResMgr::GetInst()->FindRes<CSound>(L"sound\\jug\\ChaudronAnimFall1.wav")->Play(1, 0.5f, false);
}

void JugPhase_Dead::Exit()
{
}

void JugPhase_Dead::Update()
{
	CState::Update();
}

void JugPhase_Dead::LateUpdate()
{
	CState::LateUpdate();
}
