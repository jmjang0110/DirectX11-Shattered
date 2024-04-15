#include "pch.h"
#include "JugPhase_1.h"

// [ SCRIPT PART ]
#include "BossJugCombatMgrScript.h"
#include "BossJugScript.h"
#include "BossJugHandScript.h"

JugPhase_1::JugPhase_1()
	: CState(L"JUG_PHASE_1")
	, m_pCombatMgr(nullptr)
{
}

JugPhase_1::JugPhase_1(const JugPhase_1& _origin)
	: CState(_origin.m_sStateType)
	, m_pCombatMgr(_origin.m_pCombatMgr)
{
}

JugPhase_1::~JugPhase_1()
{
}

void JugPhase_1::Init()
{
	m_pCombatMgr = GetOwner()->GetScript<BossJugCombatMgrScript>();
	assert(m_pCombatMgr);
}

void JugPhase_1::Enter()
{
	m_pCombatMgr->GetJugHand()->Activate();
}

void JugPhase_1::Exit()
{
	m_pCombatMgr->GetJugHand()->Deactivate();
}

void JugPhase_1::Update()
{
	CState::Update();
}

void JugPhase_1::LateUpdate()
{
	CState::LateUpdate();
}
