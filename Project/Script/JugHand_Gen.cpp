#include "pch.h"
#include "JugHand_Gen.h"

#include "BossJugHandScript.h"
#include "PaperBurnScript.h"

// Engine
#include <Engine/CAnimation3D.h>
#include <Engine/CAnimator3D.h>

JugHand_Gen::JugHand_Gen()
	: CState{ L"GEN" }
{
}

JugHand_Gen::~JugHand_Gen()
{
}

void JugHand_Gen::Enter()
{
	CState::ResetTimer();

	GetOwner()->Activate();

	CScript* pPaperburnScript = GetOwner()->GetScript<PaperBurnScript>();

	((PaperBurnScript*)pPaperburnScript)->On();
	((PaperBurnScript*)pPaperburnScript)->SetDir(-1.f);
	((PaperBurnScript*)pPaperburnScript)->SetStrength(1.f);

	CGameObject* pMgrObject = GetOwner()->GetParent();

	CScript* pMgrScript = pMgrObject->GetScript<HandStateMgrScript>();

	m_fLerfTime = 5.f;

	CScript* pScript = pScript = GetOwner()->GetScript<BossJugHandScript>();
	int iIndex = ((BossJugHandScript*)pScript)->GetHandIndexNumber();
	wstring sAnimName = L"";



	if (nullptr == m_pAnimation)
	{
		map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();

		if (1 == iIndex)
		{
			sAnimName = L"Hand01_Smashing";
		}
		else if (2 == iIndex)
		{
			sAnimName = L"Hand02_Idle";
		}
		else
		{
			sAnimName = L"Hand03_Idle";
		}

		if (L"" == sAnimName)
			assert(L"Animation Name is not Setting");

		if (mapAnim.find(sAnimName) == mapAnim.end())
			assert(nullptr && L"Hand State Script error");

		m_pAnimation = mapAnim.find(sAnimName)->second;
	}

	sAnimName = m_pAnimation->GetName();

	if (1 == iIndex)
	{
		((BossJugHandScript*)pScript)->SetRunningTime(0.5f);
		GetOwner()->Animator3D()->Play(sAnimName, false);
		m_pAnimation->SetAnimState(ANIMATION_STATE::STOP);

		Vec3 HandMgrPos = GetOwner()->GetParent()->Transform()->GetRelativePos();
		Vec3 pos = ((BossJugHandScript*)pScript)->GetPlayerPosition();
		pos.y += 200.f;
		GetOwner()->GetParent()->Transform()->SetRelativePos(pos);
	}
	else if (2 == iIndex)
	{
		((BossJugHandScript*)pScript)->SetRunningTime(5.f);
		GetOwner()->Animator3D()->Play(sAnimName, true);
		Vec3 pos = Vec3(0.f, 350.f, 0.f);
		GetOwner()->GetParent()->Transform()->SetRelativePos(pos);

	}
	else
	{
		((BossJugHandScript*)pScript)->SetRunningTime(5.f);
		GetOwner()->Animator3D()->Play(sAnimName, true);
		Vec3 pos = Vec3(0.f, 200.f, -100.f);
		GetOwner()->GetParent()->Transform()->SetRelativePos(pos);

	}

	CResMgr::GetInst()->FindRes<CSound>(L"sound\\jug\\JuguAppearHand1.wav")->Play(1, 0.5f, false);
}

void JugHand_Gen::Update()
{
	CState::Update();


	CGameObject* pobj = GetOwner();

	CScript* pScript = GetOwner()->GetScript<BossJugHandScript>();

	if (((BossJugHandScript*)pScript)->GetRunningTime() < CState::GetTimer())
	{
		((BossJugHandScript*)pScript)->SetGenStateDone(true);
		return;
	}

	CScript* pPaperburnScript = GetOwner()->GetScript<PaperBurnScript>();
	if (pPaperburnScript)
	{
		if (((PaperBurnScript*)pPaperburnScript)->GetStrength() <= 0.f &&
			((PaperBurnScript*)pPaperburnScript)->IsFinish() == false)
		{
			((PaperBurnScript*)pPaperburnScript)->Off();

		}
	}
}

void JugHand_Gen::Exit()
{

}

