#include "pch.h"
#include "CPlayerDieState.h"

#include <Engine/CAnimator3D.h>
#include <Engine/CFSM.h>
#include "PaperBurnScript.h"
#include "PlayerScript.h"
#include <Engine/CRigidBody.h>
#include <Engine/CNaviAgent.h>
#include "CStateMgr.h"



CPlayerDieState::CPlayerDieState()
	: CState(L"DIE")
{
}

CPlayerDieState::CPlayerDieState(const CPlayerDieState& _origin)
	:CState(_origin)

{
}

CPlayerDieState::~CPlayerDieState()
{
}


void CPlayerDieState::Enter()
{
	CState::ResetTimer();
	m_eState = DIE_STATE::DIE_START;
	PlayDieAnim(L"stun", false, 0.8f);
	PlayerScript* pScript = (PlayerScript*)GetOwner()->GetScriptByName(L"PlayerScript");
	pScript->GetStateMgr()->ChangeCurStateType(L"DIE");

	m_bPaperBurnStart = false;
	m_bPaperBurnStart2 = false;

	if (m_pPaperBurn == nullptr)
	{
		m_pPaperBurn = GetOwner()->GetScript<PaperBurnScript>();
	}


}

void CPlayerDieState::Exit()
{
	if (m_pPaperBurn)
	{
		m_pPaperBurn->Off();
	}

	
	PlayerScript* pScript = (PlayerScript*)GetOwner()->GetScriptByName(L"PlayerScript");
	pScript->GetSwordBoneCollider()->GetParent()->DeleteComponent(COMPONENT_TYPE::RIGIDBODY);
	pScript->GetSwordBoneCollider()->GetParent()->DeleteComponent(COMPONENT_TYPE::NAVIAGENT);
	pScript->GetSwordBoneCollider()->GetParent()->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));


}

void CPlayerDieState::Update()
{
	CState::Update();

	switch (m_eState)
	{
	case DIE_STATE::DIE_START:
	{
		if (m_pCurAnim != nullptr && m_pCurAnim->IsFinish())
		{

			m_eState = DIE_STATE::DIE_END;
			PlayerScript* pScript = (PlayerScript*)GetOwner()->GetScriptByName(L"PlayerScript");
			pScript->GetSwordBoneCollider()->GetParent()->AddComponent(new CRigidBody);
			pScript->GetSwordBoneCollider()->GetParent()->AddComponent(new CNaviAgent);
			pScript->GetSwordBoneCollider()->GetParent()->NaviAgent()->SetOffsetSize(Vec3(100.f, 100.f ,100.f));
			pScript->GetSwordBoneCollider()->GetParent()->NaviAgent()->SetOffsetPos(Vec3(0.f, 0.f, 0.f));
		}

	}
	break;
	case DIE_STATE::DIE_END:
	{
		if (m_pPaperBurn->IsApply() == false)
		{
			m_bPaperBurnStart = true;
			m_pPaperBurn->On();
			m_pPaperBurn->SetColor(WHITE);
		}

		if (m_bPaperBurnStart == true )
		{
			if (m_pPaperBurn->GetStrength() >= 1.f)
			{
				m_pPaperBurn->SetDir(-1);
				m_bPaperBurnStart2 = true;
			}
			if (m_bPaperBurnStart2 == true)
			{
				if (m_pPaperBurn->GetStrength() <= 0.f)
				{
					GetOwner()->FSM()->ChangeState(L"IDLE");
					PlayerScript* pPlayerScript = (PlayerScript*)GetOwner()->GetScriptByName(L"PlayerScript");
					if (pPlayerScript)
					{
						pPlayerScript->Stat_Up(STAT_TYPE::HP, 1.f);
					}
				}
			}


		}
	}
	break;

	}
}

void CPlayerDieState::LateUpdate()
{
	CState::LateUpdate();

	switch (m_eState)
	{
	case DIE_STATE::DIE_START:
	{

	}
	break;
	case DIE_STATE::DIE_END:
	{

	}
	break;

	}

}

void CPlayerDieState::PlayDieAnim(wstring _sName, bool _bRepeat, float _fSpeed)
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
