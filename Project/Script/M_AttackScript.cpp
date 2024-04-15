#include "pch.h"
#include "M_AttackScript.h"

#include "PlayerScript.h"

M_AttackScript::M_AttackScript()
	: CScript{ (int)SCRIPT_TYPE::M_ATTACKSCRIPT }
{
}

M_AttackScript::~M_AttackScript()
{
}

void M_AttackScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	// Monster collider
	GetOwner();

	if (_OtherObject->GetName() == L"player")
	{
		PlayerScript* pPlayerScript = _OtherObject->GetScript<PlayerScript>();
		if (nullptr != pPlayerScript)
			pPlayerScript->Stat_Down(STAT_TYPE::HP, 0.05f);
	}
}

void M_AttackScript::OnCollision(CGameObject* _OtherObject)
{
}

void M_AttackScript::OnCollisionExit(CGameObject* _OtherObject)
{
}
