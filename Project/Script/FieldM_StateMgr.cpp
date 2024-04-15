#include "pch.h"
#include "FieldM_StateMgr.h"
#include "FieldMonsteScript.h"

// [ EMGINE PART ] 
#include <Engine/CFSM.h>


// HOMONCULUE
#include "Homon_Die.h"
#include "Homon_Attack.h"
#include "Homon_Idle.h"
#include "Homon_Walk.h"
#include "Homon_Hit.h"

// DEUXIEMIE
#include "Deux_Die.h"
#include "Deux_Attack.h"
#include "Deux_Idle.h"
#include "Deux_Walk.h"
#include "Deux_Hit.h"

FieldM_StateMgr::FieldM_StateMgr()
	:m_pOwnerObj(nullptr)
	, m_sPrevState()
	, m_sCurState()
	, m_sNextState()
	, m_fRunTime(-1.f)
{
}

FieldM_StateMgr::~FieldM_StateMgr()
{
}

void FieldM_StateMgr::update()
{
	m_sPrevState = L"";
	m_sPrevState = m_sCurState;
	if (L"" != m_sNextState)
	{
		m_sCurState = m_sNextState;
		m_sNextState = L"";
		ChangeState(m_sCurState);
	}
	else
	{
		if (m_sPrevState == m_sCurState)
			return;
	}

}

void FieldM_StateMgr::lateUpdate()
{
}

void FieldM_StateMgr::InitState(CGameObject* _pOwner)
{
	m_pOwnerObj = _pOwner;
	CFSM* pFSM = m_pOwnerObj->FSM();

	if (pFSM == nullptr)
		return;

	CScript* monsterScript = _pOwner->GetScript<FieldMonsteScript>();

	if (nullptr == monsterScript)
		return;

	FieldMonsterType type = ((FieldMonsteScript*)monsterScript)->GetFieldMonsterType();

	if (type == FieldMonsterType::DEUXIEMIE)
	{
		pFSM->AddState(L"ATTACK", new Deux_Attack);
		pFSM->AddState(L"IDLE", new Deux_Idle);
		pFSM->AddState(L"DIE", new Deux_Die);
		pFSM->AddState(L"WALK", new Deux_Walk);
		pFSM->AddState(L"HIT", new Deux_Hit);
	}
	else if (type == FieldMonsterType::HOMONCULUS)
	{
		pFSM->AddState(L"ATTACK", new Homon_Attack);
		pFSM->AddState(L"IDLE", new Homon_Idle);
		pFSM->AddState(L"DIE", new Homon_Die);
		pFSM->AddState(L"WALK", new Homon_Walk);
		pFSM->AddState(L"HIT", new Homon_Hit);
	}
	else
	{
		return;
	}

	for (const auto& [strName, pState] : pFSM->GetAllStates())
	{
		pState->Init();
	}

	m_sCurState = L"IDLE";
}

void FieldM_StateMgr::ChangeState(wstring _sStateName)
{
	if (m_pOwnerObj == nullptr)
		return;

	CFSM* pFSM = m_pOwnerObj->FSM();
	if (pFSM == nullptr)
		return;

	pFSM->ChangeState(_sStateName);
	m_sCurState = _sStateName;
}
