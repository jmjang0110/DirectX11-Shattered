#include "pch.h"
#include "TotemScript.h"

#include "CStateMgr.h"
#include "PlayerScript.h"
#include "BossJugCombatMgrScript.h"
#include "BossJugScript.h"
#include "CObjectManager.h"

#include <Engine/CCollider3D.h>

TotemScript::TotemScript()
	: CScript{ (int)SCRIPT_TYPE::TOTEMSCRIPT }
	, m_fTotemFullHP(0.f)
	, m_fTotemCurHP(0.f)
	, m_bHitOn(false)
	, m_bPrevHitOn(false)
	, m_fPaperburnTime(2.f)
	, m_pPlayerPrevPos()
	, m_pPlayerCurPos()
	, m_fEffectTime(0.3f)
	, m_fTimer(0.f)
	, m_iShaker(1)
	, m_vOriginPos()
{
}

TotemScript::~TotemScript()
{
}

void TotemScript::HitEffect()
{
	int          iTrue = 1;
	int          iFalse = -1;

	m_fTimer += DT;

	if (m_fTimer >= m_fEffectTime)
	{
		m_fTimer = 0.f;
		m_bHit = false;

		Transform()->SetRelativePos(m_vOriginPos);

	}
	else
	{
		// 붉은색 효과

		// 진동
		Vec3 vPos = m_vOriginPos;
		vPos.x += m_iShaker * 300.f * DT;
		vPos.y += m_iShaker * 300.f * DT;

		Transform()->SetRelativePos(vPos);
		m_iShaker *= -1;
	}
}

void TotemScript::start()
{
	m_fTotemFullHP = 300.f;
	m_fTotemCurHP = 300.f;

	CTexture* pTexture = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\FBXTexture\\T_JuguArchi05Dark.png").Get();
	int iHitOn = (int)m_bHitOn;

	GetOwner()->MeshRender()->GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, pTexture);
	GetOwner()->MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_0, &iHitOn);

	m_vOriginPos = Transform()->GetRelativePos();
}

void TotemScript::update()
{
	int iHitOn = (int)m_bHitOn;
	if (m_bPrevHitOn != m_bHitOn)
	{
		GetOwner()->MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_0, &iHitOn);
		m_bPrevHitOn = m_bHitOn;
	}

	if (nullptr == m_pCombatMgr)
		m_pCombatMgr = CObjectManager::GetInst()->GetBossCombatMgr()->GetScript<
		BossJugCombatMgrScript>();

	vector<CGameObject*> pPlayerVec = CSceneMgr::GetInst()->GetCurScene()->GetLayer(L"PLAYER")->GetObjects();

	if (nullptr == m_pPlayer)
	{
		for (size_t i = 0; i < pPlayerVec.size(); i++)
		{
			if (pPlayerVec[i]->GetName() == L"player")
			{
				m_pPlayer = pPlayerVec[i];
				m_pPlayerPrevPos = m_pPlayer->Transform()->GetRelativePos();
			}
		}
	}

	if (nullptr != m_pPlayer)
	{
		float SphereRValue = GetOwner()->Collider3D()->GetOffsetScale().x;
		Vec3 TotemPos = GetOwner()->Transform()->GetRelativePos() + GetOwner()->Collider3D()->GetOffsetPos();

		m_pPlayerCurPos = m_pPlayer->Transform()->GetRelativePos();
		float distance = sqrtf((TotemPos.x - m_pPlayerCurPos.x) * (TotemPos.x - m_pPlayerCurPos.x) +
			(TotemPos.y - m_pPlayerCurPos.y) * (TotemPos.y - m_pPlayerCurPos.y) +
			(TotemPos.z - m_pPlayerCurPos.z) * (TotemPos.z - m_pPlayerCurPos.z));

		if (distance < SphereRValue)
		{
			// 플레이어 이동 제한.
			// push player
			m_pPlayer->Transform()->SetRelativePos(m_pPlayerPrevPos);
			m_pPlayerCurPos = m_pPlayer->Transform()->GetRelativePos();
		}
	}
	m_pPlayerPrevPos = m_pPlayer->Transform()->GetRelativePos();

	if (m_bHit)
	{
		HitEffect();
	}
}

void TotemScript::lateupdate()
{
	if (m_fTotemCurHP <= 0.f)
	{
		m_fPaperburnTime -= DT;

		if (m_fPaperburnTime <= 0.f)
		{
			GetOwner()->Destroy();
			return;
		}

		CMaterial* pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\PaperBurnMtrl.mtrl").Get();
		CTexture* pTexture = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\FBXTexture\\T_JuguArchi05Dark.png").Get();

		float burnStrength = (2.f - m_fPaperburnTime) / 2.f;
		int   iHitOn       = (int)false; 
		GetOwner()->MeshRender()->SetSharedMaterial(pMtrl, 0);
		GetOwner()->MeshRender()->GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, pTexture);
		GetOwner()->MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_0, &iHitOn);
		GetOwner()->MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::FLOAT_0, &burnStrength);

		GetOwner()->MeshRender()->Collider3D()->SetOffsetScale(Vec3(0, 0, 0));

	}
}

void TotemScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	CScript::OnCollisionEnter(_OtherObject);

	//if (_OtherObject->GetName() == L"Sword_Bone_Collider")
	//{
	//	PlayerScript* pPlayerScript = m_pCombatMgr->GetPlayer()->GetScript<PlayerScript>();
	//	if (nullptr != pPlayerScript)
	//	{
	//		wstring playerState = pPlayerScript->GetStateMgr()->GetCurstate();
	//		if (L"LIGHT_ATTACK" == playerState
	//			|| L"HEAVY_ATTACK" == playerState)
	//		{
	//			//BossJugScript* pJugScript = m_pCombatMgr->GetJug()->GetScript<BossJugScript>();
	//			//pJugScript->GetHit(0.06f);
	//			m_bHitOn = true;
	//			m_fTotemCurHP -= 100.f;
	//			m_bHit = true;
	//		}
	//	}
	//}

}

void TotemScript::OnCollision(CGameObject* _OtherObject)
{
	CScript::OnCollision(_OtherObject);

}

void TotemScript::OnCollisionExit(CGameObject* _OtherObject)
{
	CScript::OnCollisionExit(_OtherObject);

	if (_OtherObject->GetName() == L"Sword_Bone_Collider")
	{
		PlayerScript* pPlayerScript = m_pCombatMgr->GetPlayer()->GetScript<PlayerScript>();
		if (nullptr != pPlayerScript)
		{
			wstring playerState = pPlayerScript->GetStateMgr()->GetCurstate();
			if (L"LIGHT_ATTACK" == playerState
				|| L"HEAVY_ATTACK" == playerState)
			{
				BossJugScript* pJugScript = m_pCombatMgr->GetJug()->GetScript<BossJugScript>();
				pJugScript->GetHit(0.01f);
				m_bHitOn = true;
				m_fTotemCurHP -= 30.f;
				m_bHit = true;
			}
		}
	}

	/*	if (_OtherObject->GetName() == L"Sword_Bone_Collider")
		{
			m_bHitOn = false;
		}
		else if (_OtherObject->GetName() == L"player")
		{

		}*/
}

