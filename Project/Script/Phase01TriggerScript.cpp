#include "pch.h"
#include "Phase01TriggerScript.h"

#include "BossJugCombatMgrScript.h"
#include "CObjectManager.h"

Phase01TriggerScript::Phase01TriggerScript()
	: CScript((int)SCRIPT_TYPE::PHASE01TRIGGERSCRIPT)
	, m_pCombatMgr(nullptr)
{
}

Phase01TriggerScript::~Phase01TriggerScript()
{
}

void Phase01TriggerScript::start()
{
}

void Phase01TriggerScript::update()
{
	if (nullptr == m_pCombatMgr)
		m_pCombatMgr = CObjectManager::GetInst()->GetBossCombatMgr()->GetScript<
			BossJugCombatMgrScript>();
}

void Phase01TriggerScript::lateupdate()
{
}

void Phase01TriggerScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (L"player" == _OtherObject->GetName())
	{
		m_pCombatMgr->Phase1Enter();
	}
}

void Phase01TriggerScript::OnCollision(CGameObject* _OtherObject)
{
	CScript::OnCollision(_OtherObject);
}

void Phase01TriggerScript::OnCollisionExit(CGameObject* _OtherObject)
{
	CScript::OnCollisionExit(_OtherObject);
}
