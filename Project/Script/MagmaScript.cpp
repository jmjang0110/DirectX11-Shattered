#include "pch.h"
#include "MagmaScript.h"
#include "CObjectManager.h"
#include "M_AttackScript.h"

// < ENGINE >>
#include <Engine/CCollider3D.h>
#include <Engine/CTransform.h>
#include <Engine/CParticleSystem.h>
#include <Engine/CSerializer.h>
#include <Engine/CRenderComponent.h>

MagmaScript::MagmaScript()
	: CScript{ (int)SCRIPT_TYPE::MAGMASCRIPT }
	, m_fLifeTime(0.f)
	, m_fAddTime(0.f)
	, m_fExplodeScale(0.f)
	, m_bExplodeOn(false)
{
	m_fLifeTime = 6.f;
	m_fExplodeScale = 500.f;
}

MagmaScript::~MagmaScript() {}

void MagmaScript::update()
{
	m_fAddTime += DT;
	float nonAplha = 0.f;
	GetOwner()->MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::FLOAT_0, &nonAplha);

	if (m_fLifeTime + 1.f <= m_fAddTime)
	{
		GetOwner()->Destroy();
	}
	else if (m_fLifeTime <= m_fAddTime)
	{
		// ===============
		//	magma explode
		// ===============

		// magma collider


		if (false == m_bExplodeOn)
		{
			// explode particle 추가
			CPrefab* pPrefab = CResMgr::GetInst()->Load<CPrefab>(L"prefab\\explosion.pref", L"prefab\\explosion.pref").Get();
			CGameObject* pParticle = pPrefab->Instantiate();
			pParticle->SetName(L"magma explode effect");
			pParticle->Transform()->SetRelativePos(GetOwner()->Transform()->GetRelativePos());
			pParticle->ParticleSystem()->SetLifeTime(5.f);
			pParticle->ParticleSystem()->SetParticlePlayOneTime();
			pParticle->ParticleSystem()->SetMaterial(L"material\\explosion.mtrl");
			CSceneMgr::GetInst()->SpawnObject(pParticle, 1);

			GetOwner()->AddComponent(new M_AttackScript);
			CObjectManager::GetInst()->CreateAttackCollider(1.f, m_fExplodeScale, GetOwner()->Transform()->GetRelativePos());

			CResMgr::GetInst()->FindRes<CSound>(L"sound\\jug\\Explo1.wav")->Play(1, 0.5f, false);
			m_bExplodeOn = true;
		}

	}
	else if (4.f < m_fAddTime && m_fAddTime < 6.f)
	{
		// =====================
		//	magma explode ready
		// =====================

		// 점점 하얗게 만들어주는 trigger On
		float fAplha = (m_fAddTime - 4.f) * 4.f;;
		const Ptr<CMaterial> pMaterial = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\MagmaMtrl.mtrl");
		const Ptr<CTexture> pmagmaTex = CResMgr::GetInst()->Load<CTexture>(L"texture\\FBXTexture\\T_Lava02.png",
			L"texture\\FBXTexture\\T_Lava02.png");

		GetOwner()->MeshRender()->SetSharedMaterial(pMaterial, 0);
		GetOwner()->MeshRender()->GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, pmagmaTex);
		GetOwner()->MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::FLOAT_0, &fAplha);

	}
	else if (2.f < m_fAddTime && m_fAddTime <= 4.f)
	{
		// ================
		//	magma scale up
		// ================

		Vec3 vScale = GetOwner()->Transform()->GetRelativeScale();

		float fMagmaScale = 0.f;
		fMagmaScale = (2.f - (4.f - m_fAddTime)) / 2.f;

		vScale.y = fMagmaScale;

		GetOwner()->Transform()->SetRelativeScale(vScale);
	}
	else
	{
		// ==================
		//	magma not pop up
		// ==================
	}
}

void MagmaScript::lateupdate() {}

void MagmaScript::Serialize(YAML::Emitter& emitter)
{
	emitter << YAML::Key << NAME_OF(m_fLifeTime) << YAML::Value << m_fLifeTime;
	emitter << YAML::Key << NAME_OF(m_fAddTime) << YAML::Value << m_fAddTime;
	emitter << YAML::Key << NAME_OF(m_fExplodeScale) << YAML::Value << m_fExplodeScale;

	CScript::Serialize(emitter);
}

void MagmaScript::Deserialize(const YAML::Node& node)
{
	m_fLifeTime = node[NAME_OF(m_fLifeTime)].as<float>();
	m_fAddTime = node[NAME_OF(m_fAddTime)].as<float>();
	m_fLifeTime = node[NAME_OF(m_fLifeTime)].as<float>();

	CScript::Deserialize(node);
}
