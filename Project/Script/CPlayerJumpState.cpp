#include "pch.h"
#include "CPlayerJumpState.h"


#include "CStateMgr.h"
#include "PlayerScript.h"
#include "CPlayerStat.h"
#include "TrailScript.h"
#include "PaperBurnScript.h"






#include <Engine/CAnimator3D.h>
#include <Engine/CFSM.h>
#include <Engine/CMeshData.h>
#include <Engine/CRigidBody.h>
#include <Engine/CParticleSystem.h>


CPlayerJumpState::CPlayerJumpState()
	: CState(L"JUMP")
	, m_pStateMgr(nullptr)
	, m_iJumpCnt(0)
{
}

CPlayerJumpState::CPlayerJumpState(const CPlayerJumpState& _origin)
	:CState(_origin)
	, m_pStateMgr(nullptr)
	, m_iJumpCnt(0)

{
}

CPlayerJumpState::~CPlayerJumpState()
{
	SAFE_DELETE(m_pTranslateMgr);

}


void CPlayerJumpState::Enter()
{
	CState::ResetTimer();

	// [ 이동 관련 변수 초기화 ]
	CScript* pScript = CState::GetOwner()->GetScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT);
	PlayerScript* pPlayerScript = (PlayerScript*)pScript;

	// particle 추가
	CPrefab* pPrefab = CResMgr::GetInst()->Load<CPrefab>(L"prefab\\player_sprint.pref", L"prefab\\player_sprint.pref").Get();
	CGameObject* pParticle = pPrefab->Instantiate();
	pParticle->ParticleSystem()->SetLifeTime(3.f);
	pParticle->ParticleSystem()->SetParticlePlayOneTime();
	pParticle->ParticleSystem()->SetMaterial(L"material\\player_sprint.mtrl");
	GetOwner()->AddChild(pParticle);

	if (m_pTranslateMgr == nullptr)
	{
		CGameObject* pCamera = pPlayerScript->GetCamera();
		m_pTranslateMgr = new CTranslateMgr;
		m_pTranslateMgr->Init(GetOwner(), pCamera, pScript);

	}

	// [ 점프 관련 변수 초기화 ]
	m_iJumpCnt = 0;
	m_bJumpAir_AnimStart = false;
	m_bJumpDown_AnimStart = false;
	m_eCurState = JUMP_STATE::JUMP_UP;
	m_eJumpState = JUMP_STATE::FIRST_JUMP;


	// 애니메이션 
	//PlayJumpAnim(L"jump_2_up", false);


	// StateMGr 에 현재상태가 변경됨을 알린다. 
	m_pStateMgr = pPlayerScript->GetStateMgr();
	m_pStateMgr->ChangeCurStateType(L"JUMP");

	pPlayerScript->GetSwordBoneCollider()->Deactivate();

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

void CPlayerJumpState::Exit()
{


}

void CPlayerJumpState::Update()
{

	CState::Update();
	m_tCurKeyInfo = m_pStateMgr->GetCurKeyInfo();
	m_tPrevKeyInfo = m_pStateMgr->GetPrevKeyInfo();

	// [점프]한다. ( FIRST / SECOND JUMP )
	UpdateJump();

	// 점프 [상태] 업데이트 ( UP -> AIR -> DOWN )
	UpdateJumpState();
	UpdateJumpAnimation();

	// 방향키에 따라 [이동]한다. 
	m_pTranslateMgr->Update();	// [ 점프와 동시에 [이동/회전] 가능 ]

}

void CPlayerJumpState::LateUpdate()
{

	CState::LateUpdate();

	m_pTranslateMgr->LateUpdate();

}

void CPlayerJumpState::Jump(Vec3 _velocity)
{

	if (m_iJumpCnt > 2)
		return;

	//RigidBodyScript* pRigid = (RigidBodyScript*)GetOwner()->GetScriptByName(L"RigidBodyScript");
	CRigidBody* pRigid = GetOwner()->RigidBody();

	if (pRigid)
	{
		pRigid->SetOnGround(false);
		pRigid->SetVelocity(Vec3(0.f, 0.f, 0.f));
		pRigid->AddVelocity(_velocity); // 위로 속도를 즉시 올린다. ( 빠르게 힘을 주면서 올라감 )
		m_iJumpCnt++;					// 점프 횟수를 늘린다.
	}

}

void CPlayerJumpState::ResetForDoubleJump()
{
	// [ 점프 관련 변수 초기화 ]
	m_bJumpAir_AnimStart = false;
	m_bJumpDown_AnimStart = false;
	m_eCurState = JUMP_STATE::JUMP_UP;
	m_fPrevHeight = m_fCurHeight - 1.f; // -1 한 이유는 단지 올라감을 나타내기 위함  
}

void CPlayerJumpState::UpdateJump()
{


	switch (m_eJumpState)
	{
		// 첫번째 점프 
	case JUMP_STATE::FIRST_JUMP:
	{
		// 더블 점프 성공 
		if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::JUMP &&
			m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::TAP &&
			m_iJumpCnt == 1)
		{
			Jump(Vec3(0.f, 900.f, 0.f));				// 점프한다.
			PlayJumpAnim(L"jump_2_down", false);	// 점프 애니메이션 
			m_eJumpState = JUMP_STATE::SECOND_JUMP;		// 더블 점프 
			m_eCurState = JUMP_STATE::JUMP_UP;			// 점프 상태 ( ↑ )

			ResetForDoubleJump();
		}

		// 점프 
		if (m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::JUMP &&
			m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap & PLAYER_KEY_OPTION::TAP &&
			m_iJumpCnt == 0)
		{
			Jump(Vec3(0.f, 1200.f, 0.f));		// 점프한다. 
		}


	}
	break;
	// 두번째 점프 ( 더블 점프 )
	case JUMP_STATE::SECOND_JUMP:
	{


	}
	break;
	// 점프 제한 
	case JUMP_STATE::LIMIT_JUMP:
	{

	}
	break;
	}

}

void CPlayerJumpState::UpdateJumpState()
{

	m_fPrevHeight   = m_fCurHeight;
	m_fCurHeight    = GetOwner()->Transform()->GetRelativePos().y;
	m_fCurHeightDir = m_fCurHeight - m_fPrevHeight;


	// [ UP -> AIR -> DOWN ]
	switch (m_eCurState)
	{
	case JUMP_STATE::NONE:
		break;
	case JUMP_STATE::JUMP_UP:
	{

		// UP -> AIR ( 올라가기 시작할 때  )
		if (m_fPrevHeight <= m_fCurHeight)
		{
			m_ePrevState = m_eCurState;
			m_eCurState = JUMP_STATE::JUMP_AIR;
		}

	}
	break;
	case JUMP_STATE::JUMP_AIR:
	{
		// AIR -> DOWN 
		CAnimator3D* pAnimator3D = GetOwner()->Animator3D();
		if (pAnimator3D != nullptr)
		{
			CAnimation3D* pCurAnim = pAnimator3D->GetCurAnim();
			// Air 애니메이션이 끝난다면 
			if (pCurAnim->IsFinish() == true)
			{
				m_ePrevState = m_eCurState;
				m_eCurState = JUMP_STATE::JUMP_DOWN;
			}
		}
	}
	break;

	case JUMP_STATE::JUMP_DOWN:
	{

		// DOWN -> STATE::IDLE
		/*GravityScript* pGravity = (GravityScript*)GetOwner()->GetScriptByName(L"GravityScript");
		if (pGravity != nullptr)
		{
			if (!pGravity->Isfalling())
				GetOwner()->FSM()->ChangeState(L"IDLE");
	
	}*/

		CRigidBody* pRigid = GetOwner()->RigidBody();
		if (pRigid)
		{
			if(pRigid->IsOnGround())
				GetOwner()->FSM()->ChangeState(L"IDLE");

		}
	}
	break;
	}



}

void CPlayerJumpState::UpdateJumpAnimation()
{

	switch (m_eCurState)
	{
	case JUMP_STATE::NONE:
		break;
	case JUMP_STATE::JUMP_UP:
	{

	}
	break;
	case JUMP_STATE::JUMP_AIR:
	{
		if (m_bJumpAir_AnimStart == false)
		{
			PlayJumpAnim(L"jump_2_air", false);
			m_bJumpAir_AnimStart = true;

		}
	}
	break;
	case JUMP_STATE::JUMP_DOWN:
	{

		if (m_bJumpDown_AnimStart == false)
		{
			PlayJumpAnim(L"jump_2_down", false, 0.5f);
			m_bJumpDown_AnimStart = true;
		}
	}
	break;

	}



}


void CPlayerJumpState::PlayJumpAnim(wstring _sName, bool _bRepeat, float _fSpeed)
{
	CAnimator3D* pAnimator3D = GetOwner()->Animator3D();
	if (pAnimator3D != nullptr)
	{
		CAnimation3D* pAnim = pAnimator3D->FindAnim(_sName);
		if (pAnim != nullptr)
		{
			pAnimator3D->Play(_sName, _bRepeat);
			pAnimator3D->SetSpeed(_fSpeed);
		}
	}
}