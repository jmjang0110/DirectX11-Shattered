#include "pch.h"
#include "FogTool.h"

#include <Engine/CRenderMgr.h>
#include <Engine/CGameObject.h>
#include <Engine/CTransform.h>
#include <Engine/CMeshRender.h>
#include <Engine/CSceneMgr.h>

FogTool::FogTool()
	: UI("##FogTool")
	, m_pFog(nullptr)
	, m_bFogOn(true)
	, m_pParticleEmissive(nullptr)
	, m_bParticleEmissiveOn(false)
	, m_vFogColor(Vec4(0.5f, 0.5f, 0.5f, 1.f)) {}

FogTool::~FogTool() {}

void FogTool::SetData() {}

void FogTool::update()
{
	if (nullptr == m_pFog)
	{
		m_pFog = new CGameObject;
		m_pFog->SetName(L"Depth of Field");
		m_pFog->AddComponent(new CTransform);
		m_pFog->AddComponent(new CMeshRender);

		CMesh*        pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh").Get();
		CMaterial*    pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\FogPostProcessMtrl.mtrl").Get();
		Ptr<CTexture> pPostProcTarget = CResMgr::GetInst()->FindRes<CTexture>(L"PostProcessTex").Get();
		Ptr<CTexture> pPositionTarget = CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex").Get();

		pMtrl->SetTexParam(TEX_PARAM::TEX_0, pPostProcTarget);
		pMtrl->SetTexParam(TEX_PARAM::TEX_1, pPositionTarget);

		m_pFog->MeshRender()->SetMesh(pMesh);
		m_pFog->MeshRender()->SetSharedMaterial(pMtrl, 0);
		CSceneMgr::GetInst()->SpawnObject(m_pFog, L"BG");
	}
	else
	{
		if (m_pFog->IsDead())
			return;

		if (m_bFogOn)
		{
			if (m_pFog->IsActive() == false)
				m_pFog->Activate();
		}
		else
		{
			if (m_pFog->IsActive() == true)
				m_pFog->Deactivate();
		}
	}

	if (nullptr == m_pParticleEmissive)
	{
		m_pParticleEmissive = new CGameObject;
		m_pParticleEmissive->SetName(L"Particle Emissive");
		m_pParticleEmissive->AddComponent(new CTransform);
		m_pParticleEmissive->AddComponent(new CMeshRender);

		CMesh*        pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh").Get();
		CMaterial*    pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\EmissivePostProcessMtrl.mtrl").Get();
		Ptr<CTexture> pPostProcTarget = CResMgr::GetInst()->FindRes<CTexture>(L"PostProcessTex").Get();
		Ptr<CTexture> pEmissiveTarget = CResMgr::GetInst()->FindRes<CTexture>(L"EmissiveTargetTex").Get();

		pMtrl->SetTexParam(TEX_PARAM::TEX_0, pPostProcTarget);
		pMtrl->SetTexParam(TEX_PARAM::TEX_1, pEmissiveTarget);

		m_pParticleEmissive->MeshRender()->SetMesh(pMesh);
		m_pParticleEmissive->MeshRender()->SetSharedMaterial(pMtrl, 0);
		CSceneMgr::GetInst()->SpawnObject(m_pParticleEmissive, L"BG");
	}
	else
	{
		if (m_pParticleEmissive->IsDead())
			return;

		if (m_bParticleEmissiveOn)
		{
			if (m_pParticleEmissive->IsActive() == false)
				m_pParticleEmissive->Activate();
		}
		else
		{
			if (m_pParticleEmissive->IsActive() == true)
				m_pParticleEmissive->Deactivate();
		}
	}
}

void FogTool::render_update()
{
	SetData();

	ImGui::Text("Turn On DoF : ");
	ImGui::SameLine();
	ImGui::Checkbox("##DOF_OnOff", &m_bFogOn);
	ImGui::ColorEdit4("##START_COLOR", m_vFogColor);
	CMaterial* pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\FogPostProcessMtrl.mtrl").Get();
	pMtrl->SetScalarParam(SCALAR_PARAM::VEC4_0, m_vFogColor);

	ImGui::Text("Turn On Particle Emissive : ");
	ImGui::SameLine();
	ImGui::Checkbox("##ParticleEmissive_OnOff", &m_bParticleEmissiveOn);
}
