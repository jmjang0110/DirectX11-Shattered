#include "pch.h"
#include "CPlayerHeavyAttackState.h"
#include "PlayerScript.h"
#include "CStateMgr.h"
#include "TrailScript.h"


#include <Engine/CFSM.h>
#include <Engine/Ccamera.h>
#include <Engine/CGameObject.h>
#include <Engine/CTransform.h>
#include <Engine/CTimeMgr.h>
#include <Engine/CAnimator3D.h>
#include "PaperBurnScript.h"



CPlayerHeavyAttackState::CPlayerHeavyAttackState()
	: CState(L"HEAVY_ATTACK")
	, m_pPlayerScript(nullptr)
	, m_pStateMgr(nullptr)
	, m_eHA_State(HEAVY_ATTACK_STATE::NONE)
	, m_bCombo1_2_Success(false)
	, m_bCombo2_3_Success(false)
	, m_bComboAtt_Start(false)

{
}

CPlayerHeavyAttackState::CPlayerHeavyAttackState(const CPlayerHeavyAttackState& _origin)
	:CState(_origin)
	, m_pPlayerScript(nullptr)
	, m_pStateMgr(nullptr)
	, m_eHA_State(HEAVY_ATTACK_STATE::NONE)
	, m_bCombo1_2_Success(false)
	, m_bCombo2_3_Success(false)
	, m_bComboAtt_Start(false)
{


}

CPlayerHeavyAttackState::~CPlayerHeavyAttackState()
{
}


void CPlayerHeavyAttackState::Enter()
{
	CState::ResetTimer();
	m_eHA_State = HEAVY_ATTACK_STATE::COMBO_1;
	m_tAttTimer.Clear();

	if (m_pPlayerScript == nullptr)
	{
		m_pPlayerScript = (PlayerScript*)GetOwner()->GetScriptByName(L"PlayerScript");
		m_pStateMgr = m_pPlayerScript->GetStateMgr();

		// StateMGr 에 현재상태가 변경됨을 알린다. 
		m_pStateMgr = m_pPlayerScript->GetStateMgr();
		m_pStateMgr->ChangeCurStateType(L"HEAVY_ATTACK");
	}

	m_pPlayerScript->GetSwordBoneCollider()->Activate();

	PlayHeavyAttackAnim(L"attack_heavy_1", false);
	m_bComboAtt_Start = true;
	m_bCombo1_2_Success = false;
	m_bCombo2_3_Success = false;

	TrailScript* pTrail = (TrailScript*)GetOwner()->GetScriptByName(L"TrailScript");
	if (pTrail)
	{
		pTrail->Off();
	}


	PaperBurnScript* pBurnScript = GetOwner()->GetScript<PaperBurnScript>();
	if (pBurnScript)
	{
		pBurnScript->Off();
		pBurnScript->SetDir(1);

	}

}

void CPlayerHeavyAttackState::Exit()
{
	m_tAttTimer.Clear();
	m_bCombo1_2_Success = false;
	m_bCombo2_3_Success = false;

	m_pPlayerScript->GetSwordBoneCollider()->Deactivate();

}

void CPlayerHeavyAttackState::Update()
{
	CState::Update();
	// 키 상태 업데이트 
	m_tCurKeyInfo = m_pStateMgr->GetCurKeyInfo();
	m_tPrevKeyInfo = m_pStateMgr->GetPrevKeyInfo();

	// 강공격 상태 업데이트 
	UpdateHeavyAttackState();

	// 강공격 애니메이션 업데이트 
	UpdateHeavyAttackAnim();
}

void CPlayerHeavyAttackState::LateUpdate()
{
	CState::LateUpdate();


}

void CPlayerHeavyAttackState::UpdateHeavyAttackState()
{
	switch (m_eHA_State)
	{
	case HEAVY_ATTACK_STATE::COMBO_1:
	{
		m_tAttTimer.fCombo1_Timer += DT;

		// 1.f 초 안에 약 공격키를 또 눌렀다면  
		if (0.3f <= m_tAttTimer.fCombo1_Timer && m_tAttTimer.fCombo1_Timer <= 1.f)
		{
			if (m_bComboAtt_Start)
				m_bComboAtt_Start = false;
			else
			{
				// 콤보 성공 
				if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::HEAVY_ATTACK &&
					m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::TAP)
					m_bCombo1_2_Success = true;
			}

		}

		if (m_bCombo1_2_Success && m_tAttTimer.fCombo1_Timer >= 1.f)
		{
			// 애니메이션 재생 후 콤보3 상태로 변환
			PlayHeavyAttackAnim(L"attack_heavy_2", false);
			m_eHA_State = HEAVY_ATTACK_STATE::COMBO_2;
		}

		// 콤보 실패 
		if (!m_bCombo1_2_Success && m_tAttTimer.fCombo1_Timer >= 1.f)
		{
			m_eHA_State = HEAVY_ATTACK_STATE::COMBO_END;

		}

	}
	break;
	case HEAVY_ATTACK_STATE::COMBO_2:
	{
		m_tAttTimer.fCombo2_Timer += DT;

		// 1.f 초 안에 약 공격키를 또 눌렀다면  
		if (0.3f <= m_tAttTimer.fCombo2_Timer && m_tAttTimer.fCombo2_Timer <= 1.f)
		{
			// 콤보 성공 
			if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::HEAVY_ATTACK &&
				m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::TAP)
				m_bCombo2_3_Success = true;
		}

		if (m_bCombo2_3_Success && m_tAttTimer.fCombo2_Timer >= 1.f)
		{
			// 애니메이션 재생 후 콤보3 상태로 변환
			PlayHeavyAttackAnim(L"attack_heavy_3", false);
			m_eHA_State = HEAVY_ATTACK_STATE::COMBO_3;
		}

		// 콤보 실패 
		if (!m_bCombo2_3_Success && m_tAttTimer.fCombo2_Timer >= 1.f)
		{
			m_eHA_State = HEAVY_ATTACK_STATE::COMBO_END;

		}
	}
	break;
	case HEAVY_ATTACK_STATE::COMBO_3:
	{
		m_tAttTimer.fCombo3_Timer += DT;

		// 약공격 콤보 전부 끝 -> IDLE 상태로 돌아감 
		if (m_pCurAnim != nullptr && m_pCurAnim->IsFinish() == true)
		{
			GetOwner()->FSM()->ChangeState(L"IDLE");
		}
	}
	break;
	case HEAVY_ATTACK_STATE::COMBO_END:
	{
		// 콤보 끝 -> IDLE 상태로 돌아감 
		if (m_pCurAnim != nullptr && m_pCurAnim->IsFinish() == true)
		{
			GetOwner()->FSM()->ChangeState(L"IDLE");
		}
	}
	break;

	}

}

void CPlayerHeavyAttackState::UpdateHeavyAttackAnim()
{
}

void CPlayerHeavyAttackState::PlayHeavyAttackAnim(wstring _sName, bool _bRepeat, float _fSpeed)
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

