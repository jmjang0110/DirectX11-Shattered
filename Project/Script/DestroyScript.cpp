#include "pch.h"
#include "DestroyScript.h"

#include <Engine/CGameObject.h>


DestroyScript::DestroyScript()
	: CScript((int)SCRIPT_TYPE::DESTROYSCRIPT)
	, m_fTimer(0.f)
	, m_eDestroyMode(DESTROY_MODE::DEAFULT)
	, m_bDestory(false)
	, m_fDeadLine_Timer(0.8f)


{
	SetName(L"DestroyScript");

}

DestroyScript::DestroyScript(const DestroyScript& _origin)
	: CScript((int)SCRIPT_TYPE::DESTROYSCRIPT)
	, m_fTimer(0.f)
	, m_eDestroyMode(_origin.m_eDestroyMode)
	, m_bDestory(_origin.m_bDestory)
	, m_fDeadLine_Timer(0.8f)



{
	SetName(L"DestroyScript");

}

DestroyScript::~DestroyScript()
{


}


void DestroyScript::start()
{
	m_fTimer = 0.f;

}

void DestroyScript::update()
{

	m_fTimer += DT;
	m_fTimerRatio = m_fTimer / m_fDeadLine_Timer;

	switch (m_eDestroyMode)
	{
	case DESTROY_MODE::DEAFULT:
	{
		if (m_bDestory)
			DestroyObject();

	}
	break;
	case DESTROY_MODE::TIMER:
	{
		if (m_fTimer >= m_fDeadLine_Timer)
		{
			DestroyObject();
		}

	}
	break;
	case DESTROY_MODE::ACTIVE:
	{

		int iMtrlCnt = GetOwner()->MeshRender()->GetMtrlCount();
		for (int i = 0; i < iMtrlCnt; ++i)
		{
			GetOwner()->MeshRender()->GetDynamicMaterial(i)->SetScalarParam(SCALAR_PARAM::FLOAT_0, &m_fTimer);
		}

	}
	break;
	}
}

void DestroyScript::lateupdate()
{
}

void DestroyScript::OnCollisionEnter(CGameObject* _OtherObject)
{
}

void DestroyScript::OnCollision(CGameObject* _OtherObject)
{
}

void DestroyScript::OnCollisionExit(CGameObject* _OtherObject)
{
}

void DestroyScript::SaveToScene(FILE* _pFile)
{
}

void DestroyScript::LoadFromScene(FILE* _pFile)
{
}

void DestroyScript::DestroyObject()
{
	GetOwner()->Destroy();
}


