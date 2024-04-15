#include "pch.h"
#include "Deux_Die.h"

#include "FieldMonsteScript.h"
#include "PaperBurnScript.h"

// engine
#include <Engine/CGameObject.h>
#include <Engine/CAnimator3D.h>
#include <Engine/CParticleSystem.h>

Deux_Die::Deux_Die()
	: CState{L"DEATH"}
	, m_pAnimation(nullptr)
	, m_pOwnerMGR(nullptr) {}

Deux_Die::~Deux_Die() {}

void Deux_Die::Enter()
{
	CState::ResetTimer();

	if (nullptr == m_pAnimation)
	{
		map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();
		if (mapAnim.find(L"Deux_Death") == mapAnim.end())
			assert(nullptr && L"Error! Deuxiemie Anim data is not loaded.");
		m_pAnimation = mapAnim.find(L"Deux_Death")->second;

		m_pOwnerMGR = (GetOwner()->GetScript<FieldMonsteScript>()->GetMonsterMGR());
	}
	GetOwner()->Animator3D()->Play(m_pAnimation->GetName(), false);

	CScript* pPaperburnScript = GetOwner()->GetScript<PaperBurnScript>();

	((PaperBurnScript*)pPaperburnScript)->On();
	((PaperBurnScript*)pPaperburnScript)->SetDir(1.f);
	((PaperBurnScript*)pPaperburnScript)->SetStrength(0.f);
	((PaperBurnScript*)pPaperburnScript)->SetFinishTime(5.f);
	((PaperBurnScript*)pPaperburnScript)->SetColor(Vec4(0.f, 0.5f, 1.f, 1.f));

	GetOwner()->MeshRender()->SetDynamicShadow(false);

	// particle Ãß°¡
	CPrefab* pPrefab = CResMgr::GetInst()->Load<CPrefab>(L"prefab\\monster_die.pref", L"prefab\\monster_die.pref").Get();
	CGameObject* pParticle = pPrefab->Instantiate();
	pParticle->Transform()->SetRelativePos(GetOwner()->Transform()->GetRelativePos());
	pParticle->ParticleSystem()->SetLifeTime(3.f);
	pParticle->ParticleSystem()->SetParticlePlayOneTime();
	pParticle->ParticleSystem()->SetMaterial(L"material\\monster_die.mtrl");
	CSceneMgr::GetInst()->SpawnObject(pParticle, GAME::LAYER::BG);

	CResMgr::GetInst()->FindRes<CSound>(L"sound\\Deux\\wavSmallsword_death.wav")->Play(1, 0.5f, false);
}

void Deux_Die::Update()
{
	CState::Update();


	if (nullptr == m_pAnimation)
		return;

	if (m_pOwnerMGR->IsRunTimeEXIST())
	{
		if (m_pOwnerMGR->GetRunTime() <= CState::GetTimer())
		{
			GetOwner()->GetScript<FieldMonsteScript>()->SetCurAnimationDone();
			GetOwner()->Destroy();
		}
	}
	else
	{
		float fAnimationLength = (float)(m_pAnimation->GetEndTime() - m_pAnimation->GetStartTime());
		if (fAnimationLength + 2.f <= CState::GetTimer())
		{
			GetOwner()->GetScript<FieldMonsteScript>()->SetCurAnimationDone();
			GetOwner()->Destroy();
		}
	}
}

void Deux_Die::Exit()
{
	m_pAnimation->Reset();
}
