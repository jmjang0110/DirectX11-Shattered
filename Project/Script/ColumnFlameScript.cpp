#include "pch.h"
#include "ColumnFlameScript.h"

//[ ENGINE ]
#include <Engine/CGameObject.h>
#include <Engine/CCollider3D.h>
#include <Engine/CDecal.h>

// [Script]
#include "BossJugCombatMgrScript.h"
#include "CObjectManager.h"
#include "CPlayerStat.h"
#include "PlayerScript.h"
#include "CameraShakeScript.h"

ColumnFlameScript::ColumnFlameScript()
	: CScript((int)SCRIPT_TYPE::COLUMNFLAMESCRIPT)
{
	AddScriptParam("FLAME SPEED", SCRIPTPARAM_TYPE::FLOAT, &m_fFlameSpeed);
	AddScriptParam("ROTATION SPEED", SCRIPTPARAM_TYPE::FLOAT, &m_fRotSpeed);
	AddScriptParam("COLOR", SCRIPTPARAM_TYPE::VEC4, &m_vColor);
	AddScriptParam("DAMAGE", SCRIPTPARAM_TYPE::FLOAT, &m_fDamage);
}

ColumnFlameScript::ColumnFlameScript(const ColumnFlameScript& _origin)
	: CScript((int)SCRIPT_TYPE::COLUMNFLAMESCRIPT)
{
}

ColumnFlameScript::~ColumnFlameScript()
{
}

void ColumnFlameScript::Init()
{
	if (nullptr == m_pDecal)
	{
		m_pDecal = new CGameObject;
		m_pDecal->SetName(L"Decal");
		m_pDecal->AddComponent(new CTransform);
		m_pDecal->AddComponent(new CDecal);

		CTexture* pTex = CResMgr::GetInst()->Load<CTexture>(L"texture\\boss\\T_FissureGradient.png",
		                                                    L"texture\\boss\\T_FissureGradient.png").Get();
		m_pDecal->Decal()->SetDecalType(DECAL_TYPE::CUBE);
		m_pDecal->Decal()->SetDecalTexture(pTex);
		m_pDecal->Decal()->SetDeferredLighting(false);

		m_pDecal->Transform()->SetRelativePos(0.f, -300.f, 0.f);
		m_pDecal->Transform()->SetRelativeScale(110.f, 20.f, 110.f);

		GetOwner()->AddChild(m_pDecal);
	}

	if (nullptr == m_pLaser_1)
	{
		m_pLaser_1 = new CGameObject;
		m_pLaser_1->SetName(L"Attack0_Laser1");
		m_pLaser_1->AddComponent(new CTransform);
		m_pLaser_1->AddComponent(new CMeshRender);

		CMesh*     pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh").Get();
		CMaterial* pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\ColumnFlameMtrl.mtrl").Get();
		CTexture*  pTex  = CResMgr::GetInst()->Load<CTexture>(L"texture\\boss\\flame_2.png",
		                                                      L"texture\\boss\\flame_2.png").Get();
		m_pLaser_1->Transform()->SetRelativeScale(60.f, 600.f, 1.f);

		pMtrl->SetTexParam(TEX_PARAM::TEX_0, pTex);

		m_pLaser_1->MeshRender()->SetMesh(pMesh);
		m_pLaser_1->MeshRender()->SetSharedMaterial(pMtrl, 0);
		GetOwner()->AddChild(m_pLaser_1);

		if (nullptr == m_pDistortion_1)
		{
			m_pDistortion_1 = new CGameObject;
			m_pDistortion_1->SetName(L"Distortion");
			m_pDistortion_1->AddComponent(new CTransform);
			m_pDistortion_1->AddComponent(new CMeshRender);

			CMesh*     pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh").Get();
			CMaterial* pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\PostProcessMtrl.mtrl").Get();
			pMtrl->SetScalarParam(SCALAR_PARAM::VEC4_0, &m_vColor);

			m_pDistortion_1->MeshRender()->SetMesh(pMesh);
			m_pDistortion_1->MeshRender()->SetSharedMaterial(pMtrl, 0);
			m_pLaser_1->AddChild(m_pDistortion_1);
		}
	}

	if (nullptr == m_pLaser_2)
	{
		m_pLaser_2 = m_pLaser_1->Clone();
		m_pLaser_2->SetName(L"Attack0_Laser2");

		Vec3 rot = m_pLaser_2->Transform()->GetRelativeRotation();
		rot.ToDegree();
		rot.y += 90;
		rot.ToRadian();
		m_pLaser_2->Transform()->SetRelativeRotation(rot);

		GetOwner()->AddChild(m_pLaser_2);
	}
}

void ColumnFlameScript::start()
{
}

void ColumnFlameScript::update()
{
	m_fTimer += DT;

	if (0.5f < m_fTimer)
	{
		if (!m_fActiveLaser)
		{
			m_pLaser_1->Activate();
			m_pLaser_2->Activate();
			Collider3D()->Activate();

			if (!CObjectManager::GetInst()->GetPlayerCamera()->GetScript<CameraShakeScript>()->IsShake())
				CObjectManager::GetInst()->GetPlayerCamera()->GetScript<CameraShakeScript>()->StartShake(0.3f);

			m_fActiveLaser = true;
		}

		Vec3 rot = m_pLaser_1->Transform()->GetRelativeRotation();
		rot.ToDegree();
		rot.y += m_fRotSpeed * DT;
		rot.ToRadian();
		m_pLaser_1->Transform()->SetRelativeRotation(rot);

		rot = m_pLaser_2->Transform()->GetRelativeRotation();
		rot.ToDegree();
		rot.y += m_fRotSpeed * DT;
		rot.ToRadian();
		m_pLaser_2->Transform()->SetRelativeRotation(rot);


		if (2.5f < m_fTimer)
		{
			GetOwner()->Deactivate();
			m_fActiveLaser = false;
			m_fTimer       = 0.f;
		}
	}
	else
	{
		if (!m_fActiveLaser)
		{
			m_pLaser_1->Deactivate();
			m_pLaser_2->Deactivate();
			Collider3D()->Deactivate();
		}
	}
}

void ColumnFlameScript::lateupdate()
{
	static float yPos = 0;
	yPos += m_fFlameSpeed * DT;

	m_pDecal->Decal()->SetColor(m_vColor);

	CMaterial* pMtrl = m_pLaser_1->MeshRender()->GetMaterial(0).Get();
	pMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_0, &yPos);
	pMtrl->SetScalarParam(SCALAR_PARAM::VEC4_0, &m_vColor);

	pMtrl = m_pLaser_2->MeshRender()->GetDynamicMaterial(0).Get();
	pMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_0, &yPos);
	pMtrl->SetScalarParam(SCALAR_PARAM::VEC4_0, Vec4(1.f, 1.f, 1.f, 1.f));

	pMtrl = m_pDistortion_1->MeshRender()->GetMaterial(0).Get();
	pMtrl->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PostProcessTex"));
	pMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_0, &yPos);
}

void ColumnFlameScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	PlayerScript* pPlayerScript = _OtherObject->GetScript<PlayerScript>();
	if (nullptr != pPlayerScript)
	{
		pPlayerScript->Stat_Down(STAT_TYPE::HP, 0.03f);
	}
}

void ColumnFlameScript::OnCollision(CGameObject* _OtherObject)
{
}

void ColumnFlameScript::OnCollisionExit(CGameObject* _OtherObject)
{
}
