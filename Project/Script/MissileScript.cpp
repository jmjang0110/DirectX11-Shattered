#include "pch.h"
#include "MissileScript.h"

#include <Engine/CTimeMgr.h>
#include <Engine/CTransform.h>


MissileScript::MissileScript()
	: CScript((int)SCRIPT_TYPE::MISSILESCRIPT)
  , m_fSpeed(0.f)
  , m_fAccTime(0.f) {}

MissileScript::~MissileScript() {}

void MissileScript::update()
{
	Vec3 vPos = Transform()->GetRelativePos();

	vPos.y += DT * m_fSpeed;

	Transform()->SetRelativePos(vPos);


	m_fAccTime += DT;

	if (m_fAccTime >= 2.f)
	{
		//GetOwner()->Destroy();
	}
}

void MissileScript::OnCollisionEnter(CGameObject* _pOtherObj)
{
	/*if (_pOtherObj->GetName() == L"ChildObject")
	{
		_pOtherObj->Destroy();
	}*/
}
