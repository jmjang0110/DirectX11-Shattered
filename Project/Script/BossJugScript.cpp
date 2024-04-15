#include "pch.h"
#include "BossJugScript.h"

// [ ENGINE PART ]
#include <Engine/CGameObject.h>
#include <Engine/CState.h>
#include <Engine/CAnimator3D.h>
#include <Engine/CAnimation3D.h>
#include <Engine/CFSM.h>
#include <Engine/CSerializer.h>
#include <Engine/CUIProgressBar.h>
// [ SCRIPT PART ] 
#include "Jug_None.h"
#include "Jug_Intro.h"
#include "Jug_Norm_Idle.h"
#include "Jug_Norm_Hit.h"
#include "Jug_Hammer_Idle.h"
#include "Jug_Hammer_Hit.h"
#include "Jug_Attack_0.h"
#include "Jug_Attack_1.h"
#include "Jug_SpawnHammer.h"
#include "Jug_WalkGround.h"
#include "Jug_Fly.h"
#include "Jug_Dead.h"
#include "CObjectManager.h"


BossJugScript::BossJugScript()
	: CScript((int)SCRIPT_TYPE::BOSSJUGSCRIPT)
	, m_pBossFSM(nullptr)
	, m_pBossAnimator(nullptr)
	, m_fHP(10000.f)
	, m_fMaxHP(10000.f)
{
	AddScriptParam("BOSS STATE", SCRIPTPARAM_TYPE::TEXT, &m_strCurState);
	AddScriptParam("CUR ANIM", SCRIPTPARAM_TYPE::TEXT, &m_strCurAnimName);
	AddScriptParam("HP", SCRIPTPARAM_TYPE::FLOAT, &m_fHP);
	AddScriptParam("MAX HP", SCRIPTPARAM_TYPE::FLOAT, &m_fMaxHP);
}

BossJugScript::BossJugScript(const BossJugScript& _origin)
	: CScript((int)SCRIPT_TYPE::BOSSJUGSCRIPT)
	, m_pBossFSM(nullptr)
	, m_pBossAnimator(nullptr)
	, m_fHP(_origin.m_fHP)
	, m_fMaxHP(_origin.m_fMaxHP)
{
}

BossJugScript::~BossJugScript()
{
}


void BossJugScript::GetHit(float _fDamageRate)
{
	SetHP(m_fHP - (m_fMaxHP * _fDamageRate));
}

void BossJugScript::Init()
{
	// 애니메이션 로드
	m_pBossAnimator = GetOwner()->Animator3D();
	if (nullptr == m_pBossAnimator)
	{
		GetOwner()->AddComponent(new CAnimator3D);
	}
	map<wstring, CAnimation3D*> mapAnim = m_pBossAnimator->GetAllAnim();
	if (1 <= mapAnim.size())
	{
		m_pBossAnimator->MakeAnimationFromTXT("BossJugAnimInfo.txt");
	}

	// 자식 오브젝트와 함께 플레이
	m_pBossAnimator->CopyAllAnimToChild();
	m_pBossAnimator->SetPlayWithChild(true);

	// 상태 초기화
	InitState();
}

void BossJugScript::InitState()
{
	m_pBossFSM = GetOwner()->FSM();

	if (nullptr == m_pBossFSM)
	{
		m_pBossFSM = new CFSM;
		GetOwner()->AddComponent(m_pBossFSM);

		m_pBossFSM->AddState(GAME::BOSS::JUG_NONE, new Jug_None);
		m_pBossFSM->AddState(GAME::BOSS::JUG_INTRO, new Jug_Intro);
		m_pBossFSM->AddState(GAME::BOSS::JUG_NORM_IDLE, new Jug_Norm_Idle);
		m_pBossFSM->AddState(GAME::BOSS::JUG_NORM_HIT, new Jug_Norm_Hit);
		m_pBossFSM->AddState(GAME::BOSS::JUG_HAMMER_IDLE, new Jug_Hammer_Idle);
		m_pBossFSM->AddState(GAME::BOSS::JUG_HAMMER_HIT, new Jug_Hammer_Hit);
		m_pBossFSM->AddState(GAME::BOSS::JUG_ATTACK_0, new Jug_Attack_0);
		m_pBossFSM->AddState(GAME::BOSS::JUG_ATTACK_1, new Jug_Attack_1);
		m_pBossFSM->AddState(GAME::BOSS::JUG_SPAWNHAMMER, new Jug_SpawnHammer);
		m_pBossFSM->AddState(GAME::BOSS::JUG_WALKGROUND, new Jug_WalkGround);
		m_pBossFSM->AddState(GAME::BOSS::JUG_FLY, new Jug_Fly);
		m_pBossFSM->AddState(GAME::BOSS::JUG_DEAD, new Jug_Dead);

		for (const auto& [strName, pState] : m_pBossFSM->GetAllStates())
		{
			// 애니메이션 연결			
			pState->Init();
		}

		m_pBossFSM->SetCurState(L"JUG_NONE");
	}
}

void BossJugScript::start()
{
	CScript::start();

	CObjectManager::GetInst()->GetBossUI()->GetChild(L"PROGRESS_BAR")->UIProgressBar()->SetPercent(m_fHP / m_fMaxHP);
}

void BossJugScript::update()
{
	// [ SCRIPT PARAM UPDATE] 
	// 현재 타입 이름
	m_strCurState = ToString(m_pBossFSM->GetCurState()->GetStateType());
	// 현재 애니메이션 이름
	CAnimation3D* pCurAnim = m_pBossAnimator->GetCurAnim();
	if (nullptr == pCurAnim)
		m_strCurAnimName = GAME::BOSS::NO_ANIM;
	else
		m_strCurAnimName = ToString(m_pBossAnimator->GetCurAnim()->GetName());
}

void BossJugScript::lateupdate()
{
	CScript::lateupdate();
}

void BossJugScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	CScript::OnCollisionEnter(_OtherObject);
}

void BossJugScript::OnCollision(CGameObject* _OtherObject)
{
	CScript::OnCollision(_OtherObject);
}

void BossJugScript::OnCollisionExit(CGameObject* _OtherObject)
{
	CScript::OnCollisionExit(_OtherObject);
}

void BossJugScript::Serialize(YAML::Emitter& emitter)
{
	emitter << YAML::Key << NAME_OF(m_fHP) << YAML::Value << m_fHP;
	emitter << YAML::Key << NAME_OF(m_fMaxHP) << YAML::Value << m_fMaxHP;
}

void BossJugScript::Deserialize(const YAML::Node& node)
{
	m_fHP = node[NAME_OF(m_fHP)].as<float>();
	m_fMaxHP = node[NAME_OF(m_fMaxHP)].as<float>();

	CScript::Deserialize(node);
}

void BossJugScript::SetHP(float _hp)
{
	m_fHP = _hp;
	CObjectManager::GetInst()->GetBossUI()->GetChild(L"PROGRESS_BAR")->UIProgressBar()->
	                           SetPercent(m_fHP / m_fMaxHP);
}