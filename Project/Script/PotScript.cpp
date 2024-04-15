#include "pch.h"
#include "PotScript.h"

#include <Engine/CState.h>
#include <Engine/CAnimator3D.h>

#include "CStateMgr.h"
#include "BossJugCombatMgrScript.h"
#include "CObjectManager.h"
#include "BossJugScript.h"
#include "CStateMgr.h"
#include "PlayerScript.h"

PotScript::PotScript()
	: CScript((int)SCRIPT_TYPE::POTSCRIPT)
	, m_pCombatMgr(nullptr)
	, m_bCanHit(false)
	, m_bHit(false)
	, m_fEffectTime(0.3f)
{
}

PotScript::PotScript(const PotScript& _origin)
	: CScript((int)SCRIPT_TYPE::POTSCRIPT)
	, m_pCombatMgr(nullptr)
	, m_bCanHit(false)
	, m_bHit(false)
	, m_fEffectTime(_origin.m_fEffectTime)
{
}

PotScript::~PotScript()
{
}


void PotScript::HitEffect()
{
	static float Timer        = 0.f;
	static int   iShaker      = 1;
	static Vec3  vOriginalPos = Transform()->GetWorldPos();
	int          iTrue        = 1;
	int          iFalse       = -1;

	Timer += DT;

	if (Timer >= m_fEffectTime)
	{
		Timer  = 0.f;
		m_bHit = false;

		MeshRender()->GetDynamicMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_1, &iFalse);
		Transform()->SetRelativePos(vOriginalPos);
		
	}
	else
	{
		// 붉은색 효과
		MeshRender()->GetDynamicMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_1, &iTrue);

		// 진동
		Vec3 vPos = vOriginalPos;
		vPos.x += iShaker * 300.f * DT;
		vPos.y += iShaker * 300.f * DT;

		Transform()->SetRelativePos(vPos);
		iShaker *= -1;
	}
}

void PotScript::start()
{
	Animator3D()->Play(L"Pot_Idle", true);
}

void PotScript::update()
{
	if (nullptr == m_pCombatMgr)
		m_pCombatMgr = CObjectManager::GetInst()->GetBossCombatMgr()->GetScript<
			BossJugCombatMgrScript>();

	if (nullptr != m_pCombatMgr)
	{
		if (GAME::BOSS::PHASE::JUG_PHASE_2 == m_pCombatMgr->GetCurPhase())
			m_bCanHit = true;
	}

	if (m_bHit)
	{
		HitEffect();
	}
}

void PotScript::lateupdate()
{
}

void PotScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (m_bCanHit)
	{
		if (L"Sword_Bone_Collider" == _OtherObject->GetName())
		{
			PlayerScript* pPlayerScript = m_pCombatMgr->GetPlayer()->GetScript<PlayerScript>();
			if (nullptr != pPlayerScript)
			{
				wstring playerState = pPlayerScript->GetStateMgr()->GetCurstate();
				if (L"LIGHT_ATTACK" == playerState
				    || L"HEAVY_ATTACK" == playerState)
				{
					BossJugScript* pJugScript = m_pCombatMgr->GetJug()->GetScript<BossJugScript>();
					pJugScript->GetHit(0.06f);

					m_bHit = true;
				}
			}
		}
	}
}

void PotScript::OnCollision(CGameObject* _OtherObject)
{
	CScript::OnCollision(_OtherObject);
}

void PotScript::OnCollisionExit(CGameObject* _OtherObject)
{
	CScript::OnCollisionExit(_OtherObject);
}

void PotScript::SaveToScene(FILE* _pFile)
{
	CScript::SaveToScene(_pFile);
}

void PotScript::LoadFromScene(FILE* _pFile)
{
	CScript::LoadFromScene(_pFile);
}
