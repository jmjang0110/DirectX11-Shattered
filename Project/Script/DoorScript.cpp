#include "pch.h"
#include "DoorScript.h"

#include <Engine/CCollider3D.h>
#include <Engine/CFSM.h>

#include "CObjectManager.h"

DoorScript::DoorScript(): CScript{(int)SCRIPT_TYPE::DOORSCRIPT} {}
DoorScript::~DoorScript() {}

void DoorScript::start()
{
	GetOwner()->Collider3D()->Deactivate();
}

void DoorScript::update()
{
	if (m_start)
	{
		m_elapsedTime += DT;
		MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::FLOAT_0, &m_elapsedTime);
	}
	if (m_bEntered)
	{
		m_start = true;

		const Ptr<CTexture> pTexture = MeshRender()->GetMaterial(0)->GetTexParam(TEX_PARAM::TEX_0);
		MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\AbsorbMtrl.mtrl"), 0);
		MeshRender()->GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, pTexture);
	}

	if (m_bOpened && false == m_bEntered)
	{
		int bShow = true;;
		MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_0, &bShow);
	}
}

void DoorScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject == CObjectManager::GetInst()->GetPlayer())
	{
		_OtherObject->FSM()->ChangeState(L"IDLE");
		CObjectManager::GetInst()->FSM()->ChangeState(L"LOADING");
		m_bEntered = true;
		CKeyMgr::GetInst()->EnableAll(false);
	}
}
