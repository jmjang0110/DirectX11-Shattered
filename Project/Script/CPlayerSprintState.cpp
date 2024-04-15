#include "pch.h"
#include "CPlayerSprintState.h"


#include <Engine/CFSM.h>
#include <Engine/Ccamera.h>
#include <Engine/CGameObject.h>
#include <Engine/CTransform.h>
#include <Engine/CTimeMgr.h>
#include <Engine/CAnimator3D.h>
#include <Engine/CRigidBody.h>

#include "PlayerScript.h"
#include "CStateMgr.h"
#include "CScriptMgr.h"
#include "TrailScript.h"
#include "PaperBurnScript.h"


CPlayerSprintState::CPlayerSprintState()
	: CState(L"SPRINT")
	, m_pStateMgr(nullptr)

{
}

CPlayerSprintState::CPlayerSprintState(const CPlayerSprintState& _origin)
	:CState(_origin)

{
}

CPlayerSprintState::~CPlayerSprintState()
{
	SAFE_DELETE(m_pTranslateMgr);
}


void CPlayerSprintState::Enter()
{
	CState::ResetTimer();


	if (m_pTranslateMgr == nullptr)
	{
		CScript* pScript = CState::GetOwner()->GetScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT);
		PlayerScript* pPlayerScript = (PlayerScript*)pScript;

		CGameObject* pCamera = pPlayerScript->GetCamera();
		m_pTranslateMgr = new CTranslateMgr;
		m_pTranslateMgr->Init(GetOwner(), pCamera, pScript);

	}


	// StateMGr 에 현재상태가 변경됨을 알린다. 
	PlayerScript* pScript = (PlayerScript*)GetOwner()->GetScriptByName(L"PlayerScript");
	m_pStateMgr = pScript->GetStateMgr();
	m_sPrevState = m_pStateMgr->GetPrevState();
	m_pStateMgr->ChangeCurStateType(L"SPRINT");

	// [ 점프 ] -> [ 스프린트 ]
	if (m_sPrevState == L"JUMP")
	{
		m_eSprintType = SPRINT_TYPE::JUMP_DASH_PREPARE;

		PlaySprintAnim(L"dash_air2", false);
		Sprint(800.f);
		GravityIgnore(true);		// 중력 무시

		CResMgr::GetInst()->FindRes<CSound>(L"sound\\player\\sprint.wav")->Play(1, 0.5f, false);
	}
	// [ 이동 / IDLE ] -> [ 스프린트 ]
	if (m_sPrevState == L"MOVE" || m_sPrevState == L"IDLE")
	{

		m_eSprintType = SPRINT_TYPE::RUN_DASH;

		PlaySprintAnim(L"roll", false);
		Sprint(800.f);

		CResMgr::GetInst()->FindRes<CSound>(L"sound\\player\\ground_sprint.wav")->Play(1, 0.5f, false);
	}

	PaperBurnScript* pBurnScript = GetOwner()->GetScript<PaperBurnScript>();
	if (pBurnScript)
	{
		pBurnScript->Off();
		pBurnScript->SetDir(1);

	}

	pScript->GetSwordBoneCollider()->Deactivate();

}

void CPlayerSprintState::Exit()
{
	GravityIgnore(false);


}

void CPlayerSprintState::Update()
{
	CState::Update();

	// 스프린트 상태 업데이트 [ JUMP / MOVE SPRINT ]
	UpdateSprintState();

	// 스프린트 애니메이션 업데이트 
	UpdateSprintAnim();

}

void CPlayerSprintState::LateUpdate()
{
	CState::LateUpdate();


}


void CPlayerSprintState::UpdateSprintState()
{
	switch (m_eSprintType)
	{
	case SPRINT_TYPE::JUMP_DASH_PREPARE:
	{
		if (m_pCurAnim != nullptr && m_pCurAnim->IsFinish() == true)
		{
			m_eSprintType = SPRINT_TYPE::JUMP_DASH_ING;
			PlaySprintAnim(L"dash_air2_ing", true, 0.8f);
		}

		// [ 플레이어 잔상 ] 재생 ON
		TrailScript* pTrail = (TrailScript*)GetOwner()->GetScriptByName(L"TrailScript");
		if (pTrail)
		{
			pTrail->SetCreateTermTime(0.3f);
			pTrail->On();
		}
	}
	break;
	case SPRINT_TYPE::JUMP_DASH_ING:
	{

		//RigidBodyScript* pRigid = (RigidBodyScript*)GetOwner()->GetScriptByName(L"RigidBodyScript");
		CRigidBody* pRigid = GetOwner()->RigidBody();
		if (pRigid->GetVelocity().Length() <= 200.f)
		{
			m_eSprintType = SPRINT_TYPE::JUMP_DASH_END;
			PlaySprintAnim(L"jump_1_down", false, 0.8f);
			GravityIgnore(false);
		}


	}
	break;
	case SPRINT_TYPE::JUMP_DASH_END:
	{

		//GravityScript* pGravity = (GravityScript*)GetOwner()->GetScriptByName(L"GravityScript");
		CRigidBody* pRigid = GetOwner()->RigidBody();

		if (pRigid)
		{
			// [ 착지 ] 
			if (pRigid->IsOnGround() == true)
			{
				GetOwner()->FSM()->ChangeState(L"IDLE");
				//RigidBodyScript* pRigid = (RigidBodyScript*)GetOwner()->GetScriptByName(L"RigidBodyScript");
				CRigidBody* pRigid = GetOwner()->RigidBody();
				Vec3 vCurVelocity = pRigid->GetVelocity();
				pRigid->SetVelocity(Vec3(0.f, vCurVelocity.y, 0.f));
			}

		}

		// [ 플레이어 잔상 ] 재생 OFF 
		TrailScript* pTrail = (TrailScript*)GetOwner()->GetScriptByName(L"TrailScript");
		if (pTrail)
		{
			pTrail->Off();
		}

	}
	break;
	case SPRINT_TYPE::RUN_DASH:
	{

		if (m_pCurAnim != nullptr && m_pCurAnim->IsFinish() == true)
		{
			GetOwner()->FSM()->ChangeState(L"IDLE");
			//RigidBodyScript* pRigid = (RigidBodyScript*)GetOwner()->GetScriptByName(L"RigidBodyScript");
			CRigidBody* pRigid = GetOwner()->RigidBody();
			Vec3 vCurVelocity = pRigid->GetVelocity();
			pRigid->SetVelocity(Vec3(0.f, vCurVelocity.y, 0.f));

		}

	}
	break;

	}
}

void CPlayerSprintState::UpdateSprintAnim()
{
	switch (m_eSprintType)
	{
	case SPRINT_TYPE::JUMP_DASH_PREPARE:
	{

	}
	break;
	case SPRINT_TYPE::JUMP_DASH_ING:
	{

	}
	break;
	case SPRINT_TYPE::RUN_DASH:
	{

	}
	break;

	}
}

void CPlayerSprintState::Sprint(float _fForce)
{

	/*
		Player가 180도 돌아 있어서 아마 반대방향이 나올 것이다.
		그러므로 180도 빼주고 계산해야한다.
		* 지금 전부 계산을 Rotation.y 값을 -180.f 한 값으로 하고 있음
	*/

	Vec3 vObjForwardAxis = m_pTranslateMgr->GetForwardAxis(Vec3(0.f, -XM_PI, 0.f));
	vObjForwardAxis.Normalize();
	//RigidBodyScript* pRigid = (RigidBodyScript*)GetOwner()->GetScriptByName(L"RigidBodyScript");
	CRigidBody* pRigid = GetOwner()->RigidBody();
	pRigid->AddVelocity(vObjForwardAxis * _fForce);

}

void CPlayerSprintState::GravityIgnore(bool _b)
{
	//GravityScript* pGravity = (GravityScript*)GetOwner()->GetScriptByName(L"GravityScript");
	//if (pGravity)
	//	pGravity->SetIgnoreGravity(_b);

	CRigidBody* pRigid = GetOwner()->RigidBody();
	if(pRigid)
		pRigid->SetIgnoreGravity(_b);

}

void CPlayerSprintState::PlaySprintAnim(wstring _sName, bool _bRepeat, float _fSpeed)
{
	CAnimator3D* pAnimator3D = GetOwner()->Animator3D();
	if (pAnimator3D != nullptr)
	{
		CAnimation3D* pAnim = pAnimator3D->FindAnim(_sName);
		m_pCurAnim = pAnim;
		if (pAnim != nullptr)
		{
			pAnimator3D->Play(_sName, _bRepeat);
			pAnimator3D->SetSpeed(_fSpeed);
		}
	}
}

