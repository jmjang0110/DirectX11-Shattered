#include "pch.h"
#include "MonsterSearchScript.h"


MonsterSearchScript::MonsterSearchScript()
	: CScript{(int)SCRIPT_TYPE::MONSTERSEARCHSCRIPT}
	, m_isPlayerInRange{false}
	, m_pTarget{nullptr}
{
}

MonsterSearchScript::~MonsterSearchScript()
{
}

void MonsterSearchScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	static CLayer* pPlayerLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(GAME::LAYER::PLAYER);
	if (_OtherObject->GetLayerIndex() == pPlayerLayer->GetLayerIdx())
	{
		m_isPlayerInRange = true;
		m_pTarget         = _OtherObject;
	}
}

void MonsterSearchScript::OnCollisionExit(CGameObject* _OtherObject)
{
	static CLayer* pPlayerLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(GAME::LAYER::PLAYER);
	if (_OtherObject->GetLayerIndex() == pPlayerLayer->GetLayerIdx())
	{
		m_isPlayerInRange = false;
		m_pTarget         = nullptr;
	}
}
