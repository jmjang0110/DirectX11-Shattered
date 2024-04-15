#include "pch.h"
#include "JugHand_Attack.h"

#include "BossJugHandScript.h"
#include "EffectScript.h"
#include "MagmaScript.h"
#include "EnergyBallScript.h"
#include "CObjectManager.h"
#include "M_AttackScript.h"
#include "PaperBurnScript.h"

// Engine
#include <Engine/CAnimation3D.h>
#include <Engine/CAnimator3D.h>
#include <Engine/CFSM.h>
#include <Engine/CCollider3D.h>
#include <Engine/CParticleSystem.h>

#define ENERGYBALL_COUNT 3

JugHand_Attack::JugHand_Attack()
	: CState{ L"ATTACK" }
	, m_pAnimation(nullptr)
	, m_bFirstAttackDone(false)
	, m_bSecondAttackDone(false)
	, m_bThirdAttackDone(false)
	, m_fLerfTime(0.f)
	, m_bLandingEffectOn(false)
	, m_vecEnergyBalls()
	, m_bMagmaOn(false)
{
}

JugHand_Attack::~JugHand_Attack()
{
}

void JugHand_Attack::Hand01Attack()
{
	// [ Hand01 Attack routine ]
	// 1. 상태 진행
	// 2. 팝업되면 player 를 추적 (~ 116 Frame)
	// 3. 추적후 내려찍을땐 player 추적 x (117 Frame ~)
	// 4. 내려찍은 후 그 자리에서 멈춘뒤 사라짐
	// 5. 공중에서 팝업후 바로 116 Frame 부터 재시작.

	CScript* pScript = pScript = GetOwner()->GetScript<BossJugHandScript>();
	Vec3 vPos = GetOwner()->GetParent()->Transform()->GetRelativePos();
	Vec3 vHandCenterPos = Vec3(0.f, 0.f, - 50.f);
	Vec3 vRotate = GetOwner()->GetParent()->Transform()->GetRelativeRotation();
	Vec3 vPlayerPos = ((BossJugHandScript*)pScript)->GetPlayerPosition();
	float fSpeed = ((BossJugHandScript*)pScript)->GetSpeed();

	bool repeat = ((BossJugHandScript*)pScript)->GetAttackRepeat();

	Vec3 vDir = vPlayerPos - vPos + vHandCenterPos;
	vDir = vDir.Normalize();

	if (repeat) // 마지막 공격이 아니다 (첫번째 공격이다)
	{
		if (((BossJugHandScript*)pScript)->GetRunningTime() < CState::GetTimer())
		{
			((BossJugHandScript*)pScript)->SetAttackStateDone(true);
			m_pAnimation->SetAnimState(ANIMATION_STATE::STOP);
			m_bFirstAttackDone = true;
			m_bLandingEffectOn = false;
		}
		else if (2.f < CState::GetTimer())
		{
			if (ANIMATION_STATE::STOP == m_pAnimation->GetState())
				m_pAnimation->SetAnimState(ANIMATION_STATE::PLAY);

			// Landing Effect N Collider
			if ((m_pAnimation->GetEndFrameIdx() - 5) <= m_pAnimation->GetCurFrameIdx())
			{
				if (false == m_bLandingEffectOn)
				{
					CObjectManager::GetInst()->CreateAttackCollider(0.5f, 300.f, vPos + vHandCenterPos);
					m_bLandingEffectOn = true;
				}
			}
			else
			{
				vPos.y -= DT * 100.f;
			}
		}
		else
		{
			vPos.x += vDir.x * DT * fSpeed;
			vPos.z += vDir.z * DT * fSpeed;
		}
	}
	else
	{
		if (ANIMATION_STATE::PLAY == m_pAnimation->GetState())
		{
			if (((BossJugHandScript*)pScript)->GetRunningTime() < CState::GetTimer())
			{
				((BossJugHandScript*)pScript)->SetAttackStateDone(true);
				GetOwner()->GetScript<BossJugHandScript>()->SetAllAttackDone(true);
				m_bFirstAttackDone = false;
				((BossJugHandScript*)pScript)->SetAttackRepeat(false);
				m_bLandingEffectOn = false;
			}
			else
			{

				// Landing Effect N Collider
				if ((m_pAnimation->GetEndFrameIdx() - 5) <= m_pAnimation->GetCurFrameIdx())
				{
					if (false == m_bLandingEffectOn)
					{
						CObjectManager::GetInst()->CreateAttackCollider(0.5f, 300.f, vPos + vHandCenterPos);
						m_bLandingEffectOn = true;
					}
				}
				else
				{
					vPos.y -= DT * 100.f;
				}
			}
		}
	}

	GetOwner()->GetParent()->Transform()->SetRelativePos(vPos);
	GetOwner()->GetParent()->Transform()->SetRelativeRotation(vRotate);

	if (true == m_bMagmaOn)
		m_bMagmaOn = false;

}

void JugHand_Attack::Hand02Attack()
{
	// [ Hand02 Attack routine ]
	// 1. 중앙에서 238 전 까지 애니메이션 진행
	// 2. vanish
	// 3. 랜덤한 위치에서 gen
	// 3. 238 부터 애니메이션 시작
	// 3. 애니메이션 끝나면 사라짐

	CScript* pScript = GetOwner()->GetScript<BossJugHandScript>();

	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 vRotate = GetOwner()->Transform()->GetRelativeRotation();

	if (ANIMATION_STATE::PLAY == m_pAnimation->GetState())
	{

		if (((BossJugHandScript*)pScript)->GetRunningTime() < CState::GetTimer())
		{
			((BossJugHandScript*)pScript)->SetAttackStateDone(true);
			GetOwner()->GetScript<BossJugHandScript>()->SetAllAttackDone(true);
			m_bFirstAttackDone = false;
			return;
		}
		if (237 == m_pAnimation->GetCurFrameIdx())
		{
			// ==========================
			//		던지는 파티클 생성  
			// ==========================

			// ==========================
			//	       magma 생성  
			// ==========================
			if (false == m_bMagmaOn)
			{
				CGameObject* pMagma = nullptr;
				wstring pMagmaName = L"";
				pMagmaName = L"meshdata//magma.mdat";
				Ptr<CMeshData> pMagmaMeshData = CResMgr::GetInst()->Load<CMeshData>(pMagmaName.c_str(),
					pMagmaName.c_str());
				pMagma = pMagmaMeshData->Instantiate();
				Vec3 playerpos = GetOwner()->GetScript<BossJugHandScript>()->GetPlayerPosition();
				playerpos.y -= 10;
				pMagma->Transform()->SetRelativePos(playerpos);
				pMagma->Transform()->SetRelativeScale(Vec3(1.f, 0.f, 1.f));
				pMagma->SetName(L"Magma");
				pMagma->AddComponent(new MagmaScript);
				CSceneMgr::GetInst()->SpawnObject(pMagma, L"MONSTER");
				m_bMagmaOn = true;
			}
			
		}

	}

	GetOwner()->Transform()->SetRelativePos(vPos);
	GetOwner()->Transform()->SetRelativeRotation(vRotate);

}


void JugHand_Attack::Hand03Attack()
{
	CScript* pScript = GetOwner()->GetScript<BossJugHandScript>();

	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 vRotate = GetOwner()->Transform()->GetRelativeRotation();
	Vec3 vPlayerPos = ((BossJugHandScript*)pScript)->GetPlayerPosition();
	Vec3 vMonsterCurDir = ((BossJugHandScript*)pScript)->GetCurDir();

	Vec2 vPlayertoMonsterDir = Vec2(vPos.x - vPlayerPos.x, vPos.z - vPlayerPos.z);
	vPlayertoMonsterDir.Normalize();

	Vec2 vNewMonsterVec = -vPlayertoMonsterDir;

	((BossJugHandScript*)pScript)->SetMonsterDir(Vec3(vNewMonsterVec.x, 0, vNewMonsterVec.y));

	float fTheta = vMonsterCurDir.x * vNewMonsterVec.x + vMonsterCurDir.z * vNewMonsterVec.y;
	fTheta = acos(fTheta);

	if (vPos.x < vPlayerPos.x)
		fTheta = fTheta + XM_PI / 2.f;
	else
		fTheta = -fTheta + XM_PI / 2.f;

	if (m_pAnimation->GetCurFrameIdx() == m_pAnimation->GetEndFrameIdx())
	{
		if (((BossJugHandScript*)pScript)->GetRunningTime() < CState::GetTimer())
		{
			((BossJugHandScript*)pScript)->SetAttackStateDone(true);
			GetOwner()->GetScript<BossJugHandScript>()->SetAllAttackDone(true);
			m_bFirstAttackDone = false;
			m_bSecondAttackDone = false;
			m_bThirdAttackDone = false;
			return;
		}
	}
	else if (120 == m_pAnimation->GetCurFrameIdx())
	{
		// ==================
		// create attack ball
		// ==================

		CGameObject* pEnergyBall = new CGameObject;
		pEnergyBall->SetName(L"ENERGYBALL");
		pEnergyBall->AddComponent(new CMeshRender);
		pEnergyBall->AddComponent(new CTransform);
		pEnergyBall->AddComponent(new CCollider3D{});
		pEnergyBall->AddComponent(new EnergyBallScript{});

		pEnergyBall->Transform()->SetRelativePos(Vec3());
		pEnergyBall->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 50.f));
		pEnergyBall->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));

		CMaterial* pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\EnergyBallMtrl.mtrl").Get();
		pEnergyBall->MeshRender()->SetSharedMaterial(pMtrl, 0);
		const Ptr<CTexture> pmagmaTex = CResMgr::GetInst()->Load<CTexture>(L"texture\\FBXTexture\\T_Lava02.png",
			L"texture\\FBXTexture\\T_Lava02.png");
		pEnergyBall->MeshRender()->GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, pmagmaTex);

		pEnergyBall->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::SPHERE);
		pEnergyBall->Collider3D()->SetOffsetScale(Vec3(50.f, 50.f, 50.f));
		pEnergyBall->Collider3D()->SetOffsetPos(Vec3(0.f, 0.f, 0.f));
		pEnergyBall->Collider3D()->SetLifeTime(-1.f);

		pEnergyBall->GetScript<EnergyBallScript>()->SetCurMode(ENERGYBALL_MODE::MISSILE);

		CPrefab* pPrefab = CResMgr::GetInst()->Load<CPrefab>(L"prefab\\energy_ball.pref",
			L"prefab\\energy_ball.pref").Get();
		CGameObject* pEnergyBallParti = pPrefab->Instantiate();
		pEnergyBallParti->ParticleSystem()->SetLifeTime(-1.f);
		pEnergyBallParti->ParticleSystem()->SetMaterial(L"material\\energy_ball.mtrl");

		pEnergyBall->AddChild(pEnergyBallParti);
		m_vecEnergyBalls.push_back(pEnergyBall);
		CSceneMgr::GetInst()->SpawnObject(pEnergyBall, GAME::LAYER::MONSTER_NON_PARRING_ATTACK);
		pEnergyBall->Deactivate();
	}
	else
	{
		vRotate.y += fTheta;
	}

	GetOwner()->Transform()->SetRelativePos(vPos);
	//GetOwner()->Transform()->SetRelativeRotation(vRotate);
}

void JugHand_Attack::Init()
{
	// 에너지볼 생성
	if (m_vecEnergyBalls.empty())
	{
		for (int i = 0; i < ENERGYBALL_COUNT; i++)
		{
			// new energyball - update 12/17 AM 03:30
			CGameObject* pEnergyBall = new CGameObject;
			pEnergyBall->SetName(L"ENERGYBALL_" + std::to_wstring(i));
			pEnergyBall->AddComponent(new CMeshRender);
			pEnergyBall->AddComponent(new CTransform);
			pEnergyBall->AddComponent(new CCollider3D{});
			pEnergyBall->AddComponent(new EnergyBallScript{});

			pEnergyBall->Transform()->SetRelativePos(Vec3());
			pEnergyBall->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 50.f));
			pEnergyBall->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));

			CMaterial* pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\EnergyBallMtrl.mtrl").Get();
			pEnergyBall->MeshRender()->SetSharedMaterial(pMtrl, 0);
			const Ptr<CTexture> pmagmaTex = CResMgr::GetInst()->Load<CTexture>(L"texture\\FBXTexture\\T_Lava02.png",
				L"texture\\FBXTexture\\T_Lava02.png");
			pEnergyBall->MeshRender()->GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, pmagmaTex);

			pEnergyBall->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::SPHERE);
			pEnergyBall->Collider3D()->SetOffsetScale(Vec3(50.f, 50.f, 50.f));
			pEnergyBall->Collider3D()->SetOffsetPos(Vec3(0.f, 0.f, 0.f));
			pEnergyBall->Collider3D()->SetLifeTime(-1.f);

			CPrefab* pPrefab = CResMgr::GetInst()->Load<CPrefab>(L"prefab\\energy_ball.pref",
				L"prefab\\energy_ball.pref").Get();
			CGameObject* pEnergyBallParti = pPrefab->Instantiate();
			pEnergyBallParti->ParticleSystem()->SetLifeTime(-1.f);
			pEnergyBallParti->ParticleSystem()->SetMaterial(L"material\\energy_ball.mtrl");

			pEnergyBall->AddChild(pEnergyBallParti);
			m_vecEnergyBalls.push_back(pEnergyBall);
			CSceneMgr::GetInst()->SpawnObject(pEnergyBall, GAME::LAYER::MONSTER_NON_PARRING_ATTACK);
			pEnergyBall->Deactivate();

		}
	}
}

void JugHand_Attack::Enter()
{
	CState::ResetTimer();

	CScript* pScript = GetOwner()->GetScript<BossJugHandScript>();

	m_fLerfTime = 2.f;

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
			sAnimName = L"Hand02_Pattern2";
		}
		else
		{
			sAnimName = L"Hand03_Pattern3";
		}

		if (mapAnim.find(sAnimName) == mapAnim.end())
			assert(nullptr && L"애니메이션 클립을 찾을 수 없습니다. \n Hand State Script error");

		m_pAnimation = mapAnim.find(sAnimName)->second;
	}

	sAnimName = m_pAnimation->GetName();

	if (3 == iIndex)
	{
		float fHand3RunTime = (float)(m_pAnimation->GetEndTime() - m_pAnimation->GetStartTime());
		fHand3RunTime *= 3.f;

		GetOwner()->Animator3D()->Play(sAnimName, true);
		m_pAnimation->SetLerpTime(0.f);
		((BossJugHandScript*)pScript)->SetRunningTime(fHand3RunTime);
	}
	else if (1 == iIndex)
	{
		m_pAnimation->SetLerpTime(0.f);

		float fRunTime = (float)(m_pAnimation->GetEndTime() - m_pAnimation->GetStartTime());

		if (true == m_bFirstAttackDone)
		{
			((BossJugHandScript*)pScript)->SetRunningTime(fRunTime);
			GetOwner()->Animator3D()->Play(sAnimName, false);
			((BossJugHandScript*)pScript)->SetAttackRepeat(false);
		}
		else
		{
			((BossJugHandScript*)pScript)->SetRunningTime(fRunTime + 2.f);
			m_pAnimation->SetAnimState(ANIMATION_STATE::STOP);
			((BossJugHandScript*)pScript)->SetAttackRepeat(true);
		}
	}
	else if (2 == iIndex)
	{
		GetOwner()->Animator3D()->Play(sAnimName, false);
		m_pAnimation->SetLerpTime(0.5f);
		if (true == m_bFirstAttackDone)
		{
			m_pAnimation->SetCurFrameIdx(238);
		}
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

	m_bMagmaOn         = false;
	m_bLandingEffectOn = false;
}

void JugHand_Attack::Update()
{
	CState::Update();

	int curAttackIndex = GetOwner()->GetParent()->GetScript<HandStateMgrScript>()->GetCurAttackHandNUM();
	int iIndex = GetOwner()->GetScript<BossJugHandScript>()->GetHandIndexNumber();


	if (curAttackIndex == iIndex)
	{
		if (1 == iIndex)
		{
			Hand01Attack();
		}
		else if (2 == iIndex)
		{
			Hand02Attack();
		}
		else
		{
			Hand03Attack();
		}
	}
}

void JugHand_Attack::Exit()
{
	m_pAnimation->Reset();
	CScript* pScript = GetOwner()->GetScript<BossJugHandScript>();

	if (nullptr == pScript)
		return;

	((BossJugHandScript*)pScript)->SetRunningTime(0);
}
