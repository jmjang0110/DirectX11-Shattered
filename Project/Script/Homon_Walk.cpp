#include "pch.h"
#include "Homon_Walk.h"

#include "FieldMonsteScript.h"

// engine
#include <Engine/CGameObject.h>
#include <Engine/CAnimator3D.h>

Homon_Walk::Homon_Walk()
	: CState{L"WALK"}
	, m_pAnimation(nullptr)
	, m_pOwnerMGR(nullptr) 
	, m_fSpeed(150.f)
	, m_vPatrolPoint(Vec3(0.f, 0.f, 0.f))
	, m_fPatrolRange(800.f)
	, m_bDetectTarget(false)
{}

Homon_Walk::~Homon_Walk() {}

void Homon_Walk::Enter()
{
	CState::ResetTimer();

	if (nullptr == m_pAnimation)
	{
		map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();
		if (mapAnim.find(L"Homon_Walk") == mapAnim.end())
			assert(nullptr && L"Error! Homonculus Anim data is not loaded.");
		m_pAnimation = mapAnim.find(L"Homon_Walk")->second;

		m_pOwnerMGR = (GetOwner()->GetScript<FieldMonsteScript>()->GetMonsterMGR());
	}
	m_pAnimation->SetLerpTime(0.1f);
	GetOwner()->Animator3D()->Play(m_pAnimation->GetName(), true);

	CResMgr::GetInst()->FindRes<CSound>(L"sound\\Homon\\StepHomonc.wav")->Play(0, 0.5f, false);
}

void Homon_Walk::Update()
{
	CState::Update();

	if (nullptr == m_pAnimation)
		return;

	if (m_pOwnerMGR->IsRunTimeEXIST())
	{
		if (m_pOwnerMGR->GetRunTime() <= CState::GetTimer())
		{
			GetOwner()->GetScript<FieldMonsteScript>()->SetCurAnimationDone();
		}
	}
	else
	{
		float fAnimationLength = (float)(m_pAnimation->GetEndTime() - m_pAnimation->GetStartTime());
		if (fAnimationLength <= CState::GetTimer())
		{
			GetOwner()->GetScript<FieldMonsteScript>()->SetCurAnimationDone();
		}
	}

	UpdatePos();

}

void Homon_Walk::Trace(CGameObject* _pTarget)
{
	if (_pTarget)
	{
		Vec3 vPlayerPos = _pTarget->Transform()->GetRelativePos();
		Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
		Vec3 vDir = XMVector3Normalize(vPlayerPos - vPos);

		// Monster ---( Dir )---> Player
		vPos += vDir * DT * m_fSpeed;
		GetOwner()->Transform()->SetRelativePos(vPos);
	}

}

void Homon_Walk::Patrol()
{
	float fRange = m_fPatrolRange;
	Vec3  vPoint = m_vPatrolPoint;
	Vec3  vPos = GetOwner()->Transform()->GetRelativePos();

	// UpdatePos 
	Vec3 vDir = XMVector3Normalize(m_vPatrolTargetPos - vPos);
	vPos += vDir * DT * m_fSpeed;
	GetOwner()->Transform()->SetRelativePos(vPos);


	Vec2 vCheckPatrolFinish = Vec2(vPos.x, vPos.z) - Vec2(m_vPatrolTargetPos.x, m_vPatrolTargetPos.z);
	if ((vCheckPatrolFinish).Length() < 10.f)
	{
		m_bPatrolFinish = true;
	}
	else
		m_bPatrolFinish = false;


	if (m_bPatrolFinish == true)
	{
		// Patrol Target Pos 갱신 
		UpdatePatrolTargetPos();
	}

	// Rotation
	FieldMonsteScript* pFM = GetOwner()->GetScript<FieldMonsteScript>();
	if (pFM)
	{
		pFM->MonsterRotation(m_vPatrolTargetPos);
	}

	return;

}

void Homon_Walk::UpdatePatrolPoint(Vec3 _vPos)
{
	m_vPatrolPoint = _vPos;

}

void Homon_Walk::UpdatePatrolTargetPos()
{
	float frandAngle = rand() % 360;
	float fRad = XMConvertToRadians(frandAngle);

	Vec3 A = Vec3(0.f, 0.f, 1.f);
	Vec3 B;
	B.x = A.x * cos(fRad) - sin(fRad);
	B.z = A.z * sin(fRad) + cos(fRad);
	XMVector3Normalize(B);


	m_vPatrolTargetPos = m_vPatrolPoint + B * m_fPatrolRange;
	m_vPatrolTargetPos.y = GetOwner()->Transform()->GetRelativePos().y;


	return;

}

void Homon_Walk::UpdatePos()
{
	FieldMonsteScript* pFM = GetOwner()->GetScript<FieldMonsteScript>();
	CGameObject* pPlayer = pFM->GetPlayer();
	bool bPrevDetectTarget = m_bDetectTarget;
	m_bDetectTarget = pFM->DetectPlayer();


	if (m_bDetectTarget)
	{
		Trace(pPlayer);
	}
	else
	{
		// Trace -> Patrol ( Patrol 기준점 변경 ) 
		if (m_bDetectTarget != bPrevDetectTarget)
		{
			Vec3 vPatrolPoint = GetOwner()->Transform()->GetRelativePos();
			UpdatePatrolPoint(vPatrolPoint);
		}

		Patrol();
	}

}

void Homon_Walk::Exit()
{
	m_pAnimation->Reset();
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\Homon\\StepHomonc.wav")->Stop();
}
