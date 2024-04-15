#include "pch.h"
#include "MonsterGunScript.h"

#include <Engine/CAnimator3D.h>
#include <Engine/CCollider3D.h>
#include <Engine/CFSM.h>

#include "CAttack_Gun.h"
#include "CDeath_Gun.h"
#include "CIdle_Gun.h"
#include "CObjectManager.h"
#include "CRun_Gun.h"
#include "CStun_Gun.h"
#include "CWalk_Gun.h"
#include "MonsterSearchScript.h"
#include "PlayerScript.h"
#include <Engine/CSerializer.h>
MonsterGunScript::MonsterGunScript()
	: CScript{(int)SCRIPT_TYPE::MONSTERGUNSCRIPT}
	, m_hp{0}
	, m_radius{400}
	, m_pSearch{nullptr}

{
	SetName(L"MonsterGunScript");
	AddScriptParam("HP", SCRIPTPARAM_TYPE::FLOAT, &m_hp);
	AddScriptParam("SEARCH RADIUS", SCRIPTPARAM_TYPE::FLOAT, &m_radius);
	AddScriptParam("PREPARE RADIUS", SCRIPTPARAM_TYPE::FLOAT, &m_prepareRange);
}

MonsterGunScript::~MonsterGunScript() {}

void MonsterGunScript::start()
{
	InitState();
	MakeSearchObject();
	//CMgrScript<CObjectManager>::GetInst()->AddScriptEvent(this, &MonsterGunScript::TestCode, 5);
	//CMgrScript<CObjectManager>::GetInst()->AddScriptEvent(this, &MonsterGunScript::TestCode2, 5, 50.f);
	Animator3D()->AddCallback(L"IDLE", 5, this, &MonsterGunScript::TestCode2, 5, 50.f);
}

void MonsterGunScript::update()
{
	/*if (m_pSearch->IsPlayerInRange())
	{
		if (FSM()->GetCurState()->GetName() == GAME::DEUXIEMIE::IDLE)
		{
			FSM()->ChangeState(GAME::DEUXIEMIE::RUN);
		}
	}*/
	if (KEY_TAP(KEY::H))
	{
		Animator3D()->GetSocket(0);
	}

	if (KEY_AWAY(KEY::H))
	{
		Animator3D()->DisableSocket();
	}
}

void MonsterGunScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	static CLayer* pPlayerLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(L"Player");
	if (_OtherObject->GetLayerIndex() == pPlayerLayer->GetLayerIdx())
	{
		CState* pPlayerState = _OtherObject->FSM()->GetCurState();

		if (pPlayerLayer->GetName() == L"HEAVY_ATTACK" || pPlayerState->GetName() == L"LIGHT_ATTACK")
		{
			//
			m_hp -= 10;
			if (m_hp <= 0)
			{
				FSM()->ChangeState(GAME::DEUXIEMIE::DEATH);
			}
			else
			{
				FSM()->ChangeState(GAME::DEUXIEMIE::RUN);
			}
		}
	}
	else { }
}

void MonsterGunScript::Serialize(YAML::Emitter& emitter)
{
	emitter << NAME_OF(m_hp) << m_hp;
	emitter << NAME_OF(m_radius) << m_radius;
}

void MonsterGunScript::Deserialize(const YAML::Node& node)
{
	m_hp     = node[NAME_OF(m_hp)].as<int>();
	m_radius = node[NAME_OF(m_radius)].as<float>();
}

void MonsterGunScript::TestCode(int a)
{
	MessageBox(nullptr, std::to_wstring(a).c_str(), L"TEST",MB_OK);
}

void MonsterGunScript::TestCode2(int a, float b)
{
	float result = a * b;
	/*MessageBox(nullptr, std::to_wstring(result).c_str(), L"TEST2",MB_OK);*/
	OutputDebugString(L"TEST");
}

void MonsterGunScript::InitState()
{
	FSM()->AddState(GAME::DEUXIEMIE::IDLE, new CIdle_Gun{this});
	FSM()->AddState(GAME::DEUXIEMIE::STUN, new CStun_Gun{});
	FSM()->AddState(GAME::DEUXIEMIE::ATTACK, new CAttack_Gun{});
	FSM()->AddState(GAME::DEUXIEMIE::DEATH, new CDeath_Gun{});
	FSM()->AddState(GAME::DEUXIEMIE::RUN, new CRun_Gun{this});
	FSM()->AddState(GAME::DEUXIEMIE::WALK, new CWalk_Gun{});
	FSM()->ChangeState(GAME::DEUXIEMIE::IDLE);
}

void MonsterGunScript::MakeSearchObject()
{
	CGameObject* pSearchObject = new CGameObject{};
	pSearchObject->SetName(L"SEARCH_SPHERE");
	pSearchObject->AddComponent(new CTransform{});
	m_pSearch = new MonsterSearchScript{};
	pSearchObject->AddComponent(m_pSearch);
	pSearchObject->AddComponent(new CCollider3D{});
	pSearchObject->Collider3D()->SetOffsetPos(Vec3{0.f, 150.f, 0.f});
	pSearchObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::SPHERE);
	pSearchObject->Collider3D()->SetOffsetScale(Vec3{m_radius * 2.f});
	CSceneMgr::GetInst()->AddChild(GetOwner(), pSearchObject);
}
