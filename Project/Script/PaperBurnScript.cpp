#include "pch.h"
#include "PaperBurnScript.h"

#include <Engine/CMaterial.h>
#include <Engine/CMeshRender.h>
#include <Engine/CTexture.h>
#include <Engine/CKeyMgr.h>


PaperBurnScript::PaperBurnScript()
	: CScript((int)SCRIPT_TYPE::PAPERBURNSCRIPT)
	, m_bApply(false)
	, m_vColor(RED)
	, m_fStrength(0.f)
	, m_iDir(1)
	, m_fFinishTime(3.f)
	, m_fSpeed(1.f)

{
	SetName(L"PaperBurnScript");

}

PaperBurnScript::PaperBurnScript(const PaperBurnScript& _origin)
	: CScript((int)SCRIPT_TYPE::PAPERBURNSCRIPT)

{
	SetName(L"PaperBurnScript");


}


PaperBurnScript::~PaperBurnScript()
{
}

void PaperBurnScript::On()
{
	CMeshRender* pMeshRender = GetOwner()->MeshRender();
	if (pMeshRender == nullptr)
		return;

	if (m_bApply == false)
		m_bApply = true;


	if (m_bApply)
	{
		m_fStrength = 0.f;
		int MtrlCnt = pMeshRender->MeshRender()->GetMtrlCount();
		for (int i = 0; i < MtrlCnt; ++i)
		{
			pMeshRender->SetUseDynamicMaterial(i, true);
			pMeshRender->GetDynamicMaterial(i)->SetUseDynamicShader(true);
			pMeshRender->GetDynamicMaterial(i)->GetDynamicShader()->SetBSType(BS_TYPE::NO_ALPHA_COVERAGE);

		}
	}

	m_bFinish = false;
}

void PaperBurnScript::Off()
{

	CMeshRender* pMeshRender = GetOwner()->MeshRender();
	if (pMeshRender == nullptr)
		return;

	m_bFinish = true;
	m_bApply = false;
	m_fStrength = 0.f;

	int MtrlCnt = GetOwner()->MeshRender()->GetMtrlCount();
	for (int i = 0; i < MtrlCnt; ++i)
	{
		pMeshRender->GetDynamicMaterial(i)->SetUseDynamicShader(false);

		pMeshRender->GetMaterial(i)->SetScalarParam(SCALAR_PARAM::INT_0, &m_bApply);
		pMeshRender->GetMaterial(i)->SetScalarParam(SCALAR_PARAM::FLOAT_0, &m_fStrength);
		pMeshRender->GetMaterial(i)->SetScalarParam(SCALAR_PARAM::VEC4_0, &m_vColor);

		//pMeshRender->SetUseDynamicMaterial(i, false);


	}


}

void PaperBurnScript::Stop()
{
	CMeshRender* pMeshRender = GetOwner()->MeshRender();
	if (pMeshRender == nullptr)
		return;

	m_bFinish = true;
	m_bApply = false;
	m_fStrength = 0.f;

}

void PaperBurnScript::ReturnOriginalState()
{
	CMeshRender* pMeshRender = GetOwner()->MeshRender();
	if (pMeshRender == nullptr)
		return;

	int MtrlCnt = pMeshRender->GetMtrlCount();
	for (int i = 0; i < MtrlCnt; ++i)
	{

		pMeshRender->GetMaterial(i)->SetScalarParam(SCALAR_PARAM::INT_0, &m_bApply);
		pMeshRender->GetDynamicMaterial(i)->GetDynamicShader()->SetBSType(BS_TYPE::DEFAULT);
		pMeshRender->GetDynamicMaterial(i)->SetUseDynamicShader(false);
		pMeshRender->SetUseDynamicMaterial(i, false);

	}
}

void PaperBurnScript::start()
{
	CMeshRender* pMeshRender = GetOwner()->MeshRender();
	if (pMeshRender == nullptr)
		return;

	int MtrlCnt = pMeshRender->GetMtrlCount();
	for (int i = 0; i < MtrlCnt; ++i)
	{

		pMeshRender->SetUseDynamicMaterial(i, true);
		pMeshRender->GetDynamicMaterial(i)->CreateDynamicShader();

	}

}

void PaperBurnScript::update()
{
	if (KEY_TAP(KEY::O))
	{
		m_bApply = !m_bApply;

		if (m_bApply)
			On();
		else
			Stop();
	}

	if (KEY_TAP(KEY::P))
	{
		Off();

	}
	if (!m_bApply)
		return;


	m_fStrength += DT * m_iDir * m_fSpeed;

	if (m_fStrength <= 0.f)
		m_fStrength = 0.f;


	if (!GetOwner()->GetRenderComponent())
		return;

	CMeshRender* pMeshRender = GetOwner()->MeshRender();
	if (pMeshRender)
	{

		int MtrlCnt = GetOwner()->MeshRender()->GetMtrlCount();
		for (int i = 0; i < MtrlCnt; ++i)
		{
			pMeshRender->GetMaterial(i)->SetScalarParam(SCALAR_PARAM::INT_0, &m_bApply);
			pMeshRender->GetMaterial(i)->SetScalarParam(SCALAR_PARAM::FLOAT_0, &m_fStrength);
			pMeshRender->GetMaterial(i)->SetScalarParam(SCALAR_PARAM::VEC4_0, &m_vColor);
		}

		if (m_fStrength >= m_fFinishTime)
		{
			m_bFinish = true;
			Off();
		}

	}



}

void PaperBurnScript::lateupdate()
{

}

void PaperBurnScript::OnCollisionEnter(CGameObject* _OtherObject)
{
}

void PaperBurnScript::OnCollision(CGameObject* _OtherObject)
{
}

void PaperBurnScript::OnCollisionExit(CGameObject* _OtherObject)
{
}

void PaperBurnScript::SaveToScene(FILE* _pFile)
{
}

void PaperBurnScript::LoadFromScene(FILE* _pFile)
{
}


