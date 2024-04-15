#include "pch.h"
#include "HandStateMgrScript.h"

// [ ENGINE PART ] 
#include <Engine/CFSM.h>
#include <Engine/CSceneMgr.h>
#include <Engine/CMeshData.h>
#include <Engine/CFSM.h>
#include <Engine/CSerializer.h>
#include <Engine/CCollider3D.h>
// [ SCRIPT PART ]
#include "BossJugHandScript.h"
#include "SocketColliderScript.h"
#include "JugHand_Attack.h"
#include "JugHand_Gen.h"
#include "JugHand_Idle.h"
#include "JugHand_None.h"
#include "JugHand_Vanish.h"
#include "PaperBurnScript.h"

#include <random>
#include <Engine/CSerializer.h>

HandStateMgrScript::HandStateMgrScript()
	: CScript{ (int)SCRIPT_TYPE::HANDSTATEMGRSCRIPT }
	, m_sNextState(L"")
	, m_sPrevState(L"")
	, m_sCurstate(L"")
	, m_pHand1(nullptr)
	, m_pHand2(nullptr)
	, m_pHand3(nullptr)
	, m_vHand1HitBoxScale(Vec3(0.f))
	, m_vHand1AttackBoxScale(Vec3(0.f))
	, m_iCurAttackHandNumber(-1)
	, m_bHand01_FirstAttackDone(false)
	, m_bHand01_SecondAttackDone(false) {}

HandStateMgrScript::~HandStateMgrScript() {}

void HandStateMgrScript::update()
{
	if (1 == m_iCurAttackHandNumber)
	{
		if (false == m_pHand1->IsActive())
		{
			m_pHand1->Activate();
		}
	}
	else if (2 == m_iCurAttackHandNumber)
	{
		if (false == m_pHand2->IsActive())
			m_pHand2->Activate();
	}
	else if (3 == m_iCurAttackHandNumber)
	{
		if (false == m_pHand3->IsActive())
			m_pHand3->Activate();
	}

	m_sPrevState = L"";
	m_sPrevState = m_sCurstate;
	if (L"" != m_sNextState)
	{
		m_sCurstate = m_sNextState;
		m_sNextState = L"";
		ChangeState(m_sCurstate);
	}
	else
	{
		if (m_sPrevState == m_sCurstate)
			return;
	}
}

void HandStateMgrScript::lateupdate() {}

void HandStateMgrScript::init()
{
	wstring pMeshName = L"";

	CGameObject* pHandMgr = GetOwner();

	// Create Hand Num 01
	pMeshName = L"meshdata//hand010.mdat";
	Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(pMeshName.c_str(),
		pMeshName.c_str());
	m_pHand1 = pMeshData->Instantiate();
	m_pHand1->SetName(L"Hand01");
	m_pHand1->AddComponent(new BossJugHandScript);
	m_pHand1->AddComponent(new CFSM);
	m_pHand1->AddComponent(new PaperBurnScript);
	//CSceneMgr::GetInst()->SpawnObject(m_pHand1, GetOwner()->GetLayerIndex());
	pHandMgr->AddChild(m_pHand1);

	CGameObject* pHand01Collider = new CGameObject;
	pHand01Collider->AddComponent(new CTransform);
	m_pHand1->Deactivate();
	//pHand01Collider->AddComponent(new SocketColliderScript);
	m_pHand1->AddChild(pHand01Collider);


	// Create Hand Num 02
	pMeshName = L"";
	pMeshName = L"meshdata//hand020.mdat";
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(pMeshName.c_str(),
		pMeshName.c_str());
	m_pHand2 = pMeshData->Instantiate();
	m_pHand2->SetName(L"Hand02");
	m_pHand2->AddComponent(new BossJugHandScript);
	m_pHand2->AddComponent(new CFSM);
	m_pHand2->AddComponent(new PaperBurnScript);
	m_pHand2->Deactivate();
	//CSceneMgr::GetInst()->SpawnObject(m_pHand2, GetOwner()->GetLayerIndex());
	pHandMgr->AddChild(m_pHand2);


	// Create Hand Num 03
	pMeshName = L"";
	pMeshName = L"meshdata//hand030.mdat";
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(pMeshName.c_str(),
		pMeshName.c_str());
	m_pHand3 = pMeshData->Instantiate();
	m_pHand3->SetName(L"Hand03");
	m_pHand3->AddComponent(new BossJugHandScript);
	m_pHand3->AddComponent(new CFSM);
	m_pHand3->AddComponent(new PaperBurnScript);
	m_pHand3->Deactivate();
	//CSceneMgr::GetInst()->SpawnObject(m_pHand3, GetOwner()->GetLayerIndex());
	pHandMgr->AddChild(m_pHand3);

	InitState();
	ResetCurAttackHandNumber();

	//GetOwner()->Deactivate();
}

void HandStateMgrScript::InitState()
{
	//m_pOwnerObj = _pOwner;
	CFSM* pFSM01 = m_pHand1->FSM();
	CFSM* pFSM02 = m_pHand2->FSM();
	CFSM* pFSM03 = m_pHand3->FSM();

	if (pFSM01 == nullptr || pFSM02 == nullptr || pFSM03 == nullptr)
		return;

	pFSM01->AddState(L"ATTACK", new JugHand_Attack);
	pFSM01->AddState(L"IDLE", new JugHand_Idle);
	pFSM01->AddState(L"GEN", new JugHand_Gen);
	pFSM01->AddState(L"VANISH", new JugHand_Vanish);

	for (const auto& [strName, pState] : pFSM01->GetAllStates())
	{
		// 애니메이션 연결			
		pState->Init();
	}


	pFSM02->AddState(L"ATTACK", new JugHand_Attack);
	pFSM02->AddState(L"IDLE", new JugHand_Idle);
	pFSM02->AddState(L"GEN", new JugHand_Gen);
	pFSM02->AddState(L"VANISH", new JugHand_Vanish);

	for (const auto& [strName, pState] : pFSM02->GetAllStates())
	{
		// 애니메이션 연결			
		pState->Init();
	}


	pFSM03->AddState(L"ATTACK", new JugHand_Attack);
	pFSM03->AddState(L"IDLE", new JugHand_Idle);
	pFSM03->AddState(L"GEN", new JugHand_Gen);
	pFSM03->AddState(L"VANISH", new JugHand_Vanish);

	for (const auto& [strName, pState] : pFSM03->GetAllStates())
	{
		// 애니메이션 연결			
		pState->Init();
	}

	m_sCurstate = L"GEN";
}

void HandStateMgrScript::ResetCurAttackHandNumber()
{
	//std::random_device                 rdX;
	//std::mt19937                       genX(rdX());
	//std::uniform_int_distribution<int> disX(1, 3);
	//m_iCurAttackHandNumber = disX(genX);

	if (1 == m_iCurAttackHandNumber)
		m_iCurAttackHandNumber = 2;
	else if (2 == m_iCurAttackHandNumber)
		m_iCurAttackHandNumber = 1;
	else
		m_iCurAttackHandNumber = 1;

	ChangeState(L"GEN");
}

void HandStateMgrScript::ChangeState(wstring _sStateName)
{
	CFSM* pFSM01 = m_pHand1->FSM();
	CFSM* pFSM02 = m_pHand2->FSM();
	CFSM* pFSM03 = m_pHand3->FSM();

	if (pFSM01 == nullptr || pFSM02 == nullptr || pFSM03 == nullptr)
		return;

	if (-1 == m_iCurAttackHandNumber)
		return;
	else
	{
		switch (m_iCurAttackHandNumber)
		{
		case 1:
			return pFSM01->ChangeState(_sStateName);;
		case 2:
			return pFSM02->ChangeState(_sStateName);;
		case 3:
			return pFSM03->ChangeState(_sStateName);;
		}
	}
}


void HandStateMgrScript::Serialize(YAML::Emitter& emitter)
{
	//emitter << YAML::Key << NAME_OF(m_sPrevState) << YAML::Value << m_sPrevState;
	//emitter << YAML::Key << NAME_OF(m_sCurstate) << YAML::Value << m_sCurstate;
	//emitter << YAML::Key << NAME_OF(m_sNextState) << YAML::Value << m_sNextState;
	//emitter << YAML::Key << NAME_OF(m_pHand1) << YAML::Value << m_pHand1;
	//emitter << YAML::Key << NAME_OF(m_pHand2) << YAML::Value << m_pHand2;
	//emitter << YAML::Key << NAME_OF(m_pHand3) << YAML::Value << m_pHand3;
	emitter << YAML::Key << NAME_OF(m_vHand1HitBoxScale) << YAML::Value << m_vHand1HitBoxScale;
	emitter << YAML::Key << NAME_OF(m_vHand1AttackBoxScale) << YAML::Value << m_vHand1AttackBoxScale;
	emitter << YAML::Key << NAME_OF(m_iCurAttackHandNumber) << YAML::Value << m_iCurAttackHandNumber;
	emitter << YAML::Key << NAME_OF(m_bHand01_FirstAttackDone) << YAML::Value << m_bHand01_FirstAttackDone;
	emitter << YAML::Key << NAME_OF(m_bHand01_SecondAttackDone) << YAML::Value << m_bHand01_SecondAttackDone;

	//CScript::Serialize(emitter);
}

void HandStateMgrScript::Deserialize(const YAML::Node& node)
{
	//m_sPrevState = node[NAME_OF(m_sPrevState)].as<wstring>();
	//m_sCurstate = node[NAME_OF(m_sCurstate)].as<wstring>();
	//m_sNextState = node[NAME_OF(m_sNextState)].as<wstring>();
	//m_pHand1                   = node[NAME_OF(m_pHand1)].as<CGameObject*>();
	//m_pHand2                   = node[NAME_OF(m_pHand2)].as<CGameObject*>();
	//m_pHand3                   = node[NAME_OF(m_pHand3)].as<CGameObject*>();
	m_vHand1HitBoxScale = node[NAME_OF(m_vHand1HitBoxScale)].as<Vec3>();
	m_vHand1AttackBoxScale = node[NAME_OF(m_vHand1AttackBoxScale)].as<Vec3>();
	m_iCurAttackHandNumber = node[NAME_OF(m_iCurAttackHandNumber)].as<int>();
	m_bHand01_FirstAttackDone = node[NAME_OF(m_bHand01_FirstAttackDone)].as<bool>();
	m_bHand01_SecondAttackDone = node[NAME_OF(m_bHand01_SecondAttackDone)].as<bool>();

	//CScript::Deserialize(node);
}
