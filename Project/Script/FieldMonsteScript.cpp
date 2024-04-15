#include "pch.h"
#include "FieldMonsteScript.h"
#include "FieldM_StateMgr.h"
#include "PlayerScript.h"
#include "BossJugCombatMgrScript.h"
#include "CStateMgr.h"
#include "CObjectManager.h"

#include <random>

#include <Engine/CAnimator3D.h>
#include <Engine/CAnimation3D.h>
#include <Engine/CSerializer.h>
#include <Engine/CGameObject.h>
#include <Engine/CParticleSystem.h>

namespace MONSTER
{
	constexpr float DEFAULT_DETECT_RANGE = 800.f;
	constexpr float DEFAULT_ATTACK_RANGE = 200.f;

}
FieldMonsteScript::FieldMonsteScript()
	: CScript{ (int)SCRIPT_TYPE::FIELDMONSTESCRIPT }
	, m_pMonsterMgr(nullptr)
	, m_vColliderSize()
	, m_pOwnerMonster(nullptr)
	, m_pPlayer(nullptr)
	, m_fDetachRange(0.f)
	, m_fCurDistance(0.f)
	, m_fAttackRange(0.f)
	, m_fFullHP(0.f)
	, m_fCurHP(0.f)
	, m_eMonsterType(FieldMonsterType::NONE)
	, m_bCurAnimationDone(true)
	, m_bIsChasing(false)
	, m_bChasingON(false)
	, m_fRotateSpeed(10.f)

{
	SetName(L"FieldMonsterScript");

	m_fFullHP = 500;
	m_fDetachRange = MONSTER::DEFAULT_DETECT_RANGE;
	m_fAttackRange = MONSTER::DEFAULT_ATTACK_RANGE;

	AddScriptParam("DETECT_RANGE", SCRIPTPARAM_TYPE::INT, &m_fDetachRange);
	AddScriptParam("ATTACK_RANGE", SCRIPTPARAM_TYPE::INT, &m_fAttackRange);
}

FieldMonsteScript::FieldMonsteScript(const FieldMonsteScript& _origin)
	: CScript{ (int)SCRIPT_TYPE::FIELDMONSTESCRIPT }
	, m_pMonsterMgr(_origin.m_pMonsterMgr)
	, m_vColliderSize(_origin.m_vColliderSize)
	, m_pOwnerMonster(_origin.m_pOwnerMonster)
	, m_pPlayer(_origin.m_pPlayer)
	, m_fDetachRange(_origin.m_fDetachRange)
	, m_fCurDistance(_origin.m_fCurDistance)
	, m_fAttackRange(_origin.m_fAttackRange)
	, m_fFullHP(_origin.m_fFullHP)
	, m_fCurHP(_origin.m_fCurHP)
	, m_eMonsterType(_origin.m_eMonsterType)
	, m_bCurAnimationDone(true)
	, m_bIsChasing(false)
	, m_bChasingON(false)
	, m_fRotateSpeed(10.f)

{
	SetName(L"FieldMonsterScript");
	AddScriptParam("DETECT_RANGE", SCRIPTPARAM_TYPE::INT, &m_fDetachRange);
	AddScriptParam("ATTACK_RANGE", SCRIPTPARAM_TYPE::INT, &m_fAttackRange);
}

FieldMonsteScript::~FieldMonsteScript()
{
	SAFE_DELETE(m_pMonsterMgr);
}

bool FieldMonsteScript::DetectPlayer()
{
	if (nullptr == m_pPlayer)
	{
		vector<CGameObject*> pPlayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(3)->GetObjects();

		for (size_t i = 0; i < pPlayer.size(); i++)
		{
			if (nullptr != pPlayer[i]->GetScript<PlayerScript>())
			{
				m_pPlayer = pPlayer[i];
			}
		}
	}

	if (nullptr == m_pPlayer)
		return false;

	Vec3 vPlayerPos = m_pPlayer->Transform()->GetRelativePos();
	Vec3 vMonsterPos = m_pOwnerMonster->Transform()->GetRelativePos();

	m_fCurDistance = (vPlayerPos - vMonsterPos).Length();
	if (m_fCurDistance <= m_fDetachRange)
	{
		Vec3 vPlayerPos = m_pPlayer->Transform()->GetRelativePos();
		MonsterRotation(vPlayerPos);
		return true;
	}
	else
		return false;

}

void FieldMonsteScript::MonsterRotation(Vec3 _vPos)
{
	if (L"WALK" != m_pMonsterMgr->GetCurState())
		return;

	Vec3 pCurMonsterPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 pCurMonsterRotation = GetOwner()->Transform()->GetRelativeRotation();
	Vec3 pNewMonsterRotation;

	//Vec3 vPlayerPos = m_pPlayer->Transform()->GetRelativePos();

	Vec3 vBossFront = GetOwner()->Transform()->GetWorldFrontDir();
	vBossFront *= -1;

	// 보스의 정면 벡터와 보스-플레이어 벡터 사이의 각을 구한다.
	Vec3 v1(vBossFront.x, 0.f, vBossFront.z);
	Vec3 v2(_vPos.x - pCurMonsterPos.x, 0.f, _vPos.z - pCurMonsterPos.z);

	v1.Normalize();
	v2.Normalize();

	float fCos = v1.Dot(v2);
	if (fCos >= 1.f) fCos = 1.f;
	if (fCos <= -1.f) fCos = -1.f;

	float dot = XMConvertToDegrees(acosf(fCos)); // 0 ~ 180
	Vec3  cross = v1.Cross(v2); // -1 ~ 1

	if (-5.f < dot && dot < 5.f)
		return;

	float fSpeed = 0;

	// 회전 방향을 찾는다
	if (0.f < cross.y)
		fSpeed = 80.f;
	else if (0.f > cross.y)
		fSpeed = -80.f;

	// 각이 5도 이상이면 플레이어 방향으로 지속적으로 회전하고 5도 이하이면 플레이를 바로 바라본다.
	if (fabsf(dot) > 5.f)
	{
		Vec3 vBossRot = pCurMonsterRotation;
		vBossRot.ToDegree();
		vBossRot.y += DT * fSpeed;
		vBossRot.ToRadian();
		pNewMonsterRotation = vBossRot;
	}
	else
	{
		Vec3 vBossRot = pCurMonsterRotation;
		vBossRot.ToDegree();
		vBossRot.y = dot * (cross.y ? -1.f : 1.f);
		vBossRot.ToRadian();
		pNewMonsterRotation = vBossRot;
	}


	GetOwner()->Transform()->SetRelativeRotation(pNewMonsterRotation);
}

void FieldMonsteScript::RotateY(Vec3 _vDir)
{
	Vec3 vRot   = GetOwner()->Transform()->GetRelativeRotation();

	Vec3 vFrom	= GetOwner()->Transform()->GetWorldFrontDir();
	Vec3 vTo	= _vDir;

	// Y 값 무시  
	vFrom = Vec3(vFrom.x, 0.f, vFrom.z);
	vTo   = Vec3(vTo.x, 0.f, vTo.z);
	vFrom.Normalize();
	vTo.Normalize();

	float fCos = vFrom.Dot(vTo);
	if (fCos >= 1.f) fCos = 1.f;
	if (fCos <= -1.f) fCos = -1.f;

	float fRad = GetAngleRadian(vFrom, vTo);
	Vec3 vCross = XMVector3Cross(vTo, vFrom);

	if (-5.f < fCos && fCos < 5.f)
		return;

	// 회전 방향을 찾는다
	if (0.f < vCross.y)
		m_iRotateDir = 1;
	else if (0.f > vCross.y)
		m_iRotateDir = -1;


	if (fabsf(fCos) > 5.f)
	{
		vRot.y += DT * m_iRotateDir * m_fRotateSpeed;
	}

	GetOwner()->Transform()->SetRelativeRotation(vRot);

}

float FieldMonsteScript::GetAngleRadian(Vec3 _v1, Vec3 _v2)
{
	// Degree : -180 ~ 180
	Vec3 vFrom = _v1;
	vFrom.Normalize();

	Vec3 vTo = _v2;
	vTo.Normalize();

	float fCos = vFrom.Dot(vTo);
	if (fCos >= 1.f) fCos = 1.f;
	if (fCos <= -1.f) fCos = -1.f;

	float fRad = acosf(fCos);					// 사이각 ( Radian )
	float fAng = fRad * (180.f / XM_PI);		// 사이각 ( Angle )	}

	if (fAng == -180.f)
		fRad = XM_PI;

	Vec3 vCross = XMVector3Normalize(XMVector3Cross(vTo, vFrom));



	return fRad;
}

void FieldMonsteScript::PeaceStateRotation()
{
	// peace 상태일때의 rotate
	Vec3 pMonsterRotation = GetOwner()->Transform()->GetRelativeRotation();

	std::random_device                 rdX;
	std::mt19937                       genX(rdX());
	std::uniform_int_distribution<int> disX(-60, 60);
	int angle = disX(genX);

	pMonsterRotation.y += angle;

	Vec3 vMonsterFront = GetOwner()->Transform()->GetWorldFrontDir();
	vMonsterFront *= -1;

	GetOwner()->Transform()->SetRelativeRotation(pMonsterRotation);
}

void FieldMonsteScript::start()
{
	if (nullptr == m_pOwnerMonster)
	{
		m_pOwnerMonster = GetOwner();
	}

	if (nullptr == m_pMonsterMgr)
	{
		m_pMonsterMgr = new FieldM_StateMgr;
		m_pMonsterMgr->InitState(GetOwner());
	}

	// 애니메이션 클립 정보가 없을시 직접 로드.
	map<wstring, CAnimation3D*> mapAnim = GetOwner()->Animator3D()->GetAllAnim();
	if (1 == mapAnim.size())
	{
		if (m_eMonsterType == FieldMonsterType::HOMONCULUS)
		{
			GetOwner()->Animator3D()->MakeAnimationFromTXT("HomonculusAnimInfo.txt");
		}
		else if (m_eMonsterType == FieldMonsterType::DEUXIEMIE)
		{
			GetOwner()->Animator3D()->MakeAnimationFromTXT("DeuxiemeAnimInfo.txt");
		}
		else
			return;
	}

	m_fCurHP = m_fFullHP;
	m_pMonsterMgr->SetNextState(L"IDLE");

}

void FieldMonsteScript::update()
{

	if (m_fCurHP <= 0.f)
	{
		if (m_pMonsterMgr->GetCurState() != L"DIE")
		{
			m_pMonsterMgr->ChangeState(L"DIE");
			m_pMonsterMgr->SetRunTime(-1.f);
			m_pMonsterMgr->update();
			return;
		}	
		else
		{
			return;
		}

	}

	if (m_bHit)
	{
		return;
	}

	if (m_bCurAnimationDone)
	{
		if (DetectPlayer())
		{
			if (false == m_bIsChasing)
			{
				// 방금 플레이어를 인식.
				m_pMonsterMgr->SetNextState(L"WALK");
				m_pMonsterMgr->SetRunTime(3.f);
				m_bCurAnimationDone = false;
				m_bIsChasing = true;
			}
			else
			{
				if (m_fCurDistance <= m_fAttackRange)
				{
					if (L"ATTACK" == m_pMonsterMgr->GetCurState())
					{
						// idle 모드로 전환.
						m_pMonsterMgr->SetNextState(L"IDLE");
						m_pMonsterMgr->SetRunTime(2.f);
						m_bCurAnimationDone = false;

					}
				}
				else
				{
					// 감지 범위 내이지만, 공격 범위는 아님. 계속 쫒아감
					m_pMonsterMgr->SetNextState(L"WALK");
					m_pMonsterMgr->SetRunTime(10.f);
					m_bCurAnimationDone = false;
					m_bChasingON = true;
				}
			}
		}
		else
		{
			m_bIsChasing = false;
			m_bChasingON = false;
			//  idle 하거나 walk 하거나
			// Set RunTime
			if (L"IDLE" == m_pMonsterMgr->GetCurState())
			{
				//PeaceStateRotation();
				m_pMonsterMgr->SetNextState(L"WALK");
			}
			else
				m_pMonsterMgr->SetNextState(L"IDLE");
			m_pMonsterMgr->SetRunTime(4.f);
		}
		m_bCurAnimationDone = false;

	}
	else
	{
		// 다만 attack range에 들어오면 바로 공격으로 한다.
		if (DetectPlayer())
		{
			if (L"WALK" == m_pMonsterMgr->GetCurState() && m_bChasingON)
			{
				if (m_fCurDistance <= m_fAttackRange)
				{
					m_pMonsterMgr->SetNextState(L"ATTACK");
					m_pMonsterMgr->SetRunTime(-1.f);
					m_bCurAnimationDone = false;
				}
			}
		}

		return;
	}

	m_pMonsterMgr->update();
}

void FieldMonsteScript::lateupdate()
{
	m_pMonsterMgr->lateUpdate();
}

void FieldMonsteScript::OnCollisionEnter(CGameObject* _OtherObject)
{

	if (_OtherObject->GetName() == L"Sword_Bone_Collider")
	{
		if (false == m_bHit)
		{
			vector<CGameObject*> vecPlayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(L"PLAYER")->GetObjects();
			PlayerScript* pPlayerScript = nullptr;
			for (int i = 0; i < vecPlayer.size(); i++)
			{
				if (vecPlayer[i]->GetName() == L"player")
				{
					pPlayerScript = vecPlayer[i]->GetScript<PlayerScript>();
				}
			}

			if (nullptr != pPlayerScript)
			{
				wstring playerState = pPlayerScript->GetStateMgr()->GetCurstate();
				if (L"LIGHT_ATTACK" == playerState
					|| L"HEAVY_ATTACK" == playerState)
				{
					m_fCurHP -= ((PlayerScript*)pPlayerScript)->GetPlayerStat()->GetDamage();
				}
			}

			// particle 추가
			CPrefab* pPrefab = CResMgr::GetInst()->Load<CPrefab>(L"prefab\\attack_01.pref", L"prefab\\attack_01.pref").Get();
			CGameObject* pParticle = pPrefab->Instantiate();
			pParticle->Transform()->SetRelativePos(Vec3(0, 50, 0));
			pParticle->ParticleSystem()->SetLifeTime(3.f);
			pParticle->ParticleSystem()->SetLinearParticle(true);
			pParticle->ParticleSystem()->SetParticlePlayOneTime();
			pParticle->ParticleSystem()->SetMaterial(L"material\\attack_01.mtrl");
			GetOwner()->AddChild(pParticle);

			m_bHit = true;
			m_pMonsterMgr->ChangeState(L"HIT");
			m_pMonsterMgr->SetRunTime(-1.f);
			m_bCurAnimationDone = false;
		}
	}
}

void FieldMonsteScript::OnCollision(CGameObject* _OtherObject)
{

}

void FieldMonsteScript::OnCollisionExit(CGameObject* _OtherObject)
{
}

void FieldMonsteScript::Serialize(YAML::Emitter& emitter)
{
	emitter << YAML::Key << NAME_OF(m_fDetachRange) << YAML::Value << m_fDetachRange;
	emitter << YAML::Key << NAME_OF(m_fAttackRange) << YAML::Value << m_fAttackRange;
	emitter << YAML::Key << NAME_OF(m_fFullHP) << YAML::Value << m_fFullHP;

	CScript::Serialize(emitter);
}

void FieldMonsteScript::Deserialize(const YAML::Node& node)
{
	m_fDetachRange = node[NAME_OF(m_fHP)].as<float>();
	m_fAttackRange = node[NAME_OF(m_fMaxHP)].as<float>();
	m_fFullHP = node[NAME_OF(m_fFullHP)].as<int>();

	CScript::Deserialize(node);
}