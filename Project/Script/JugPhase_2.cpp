#include "pch.h"
#include "JugPhase_2.h"

// [ ENGINE PART]
#include <Engine/CFSM.h>
#include <Engine/CState.h>
#include <Engine/CAnimator3D.h>
#include <Engine/CAnimation3D.h>
#include <Engine/CGameObject.h>
#include <Engine/CTransform.h>
#include <Engine/CCollider3D.h>
#include <Engine/CParticleSystem.h>
#include <Engine/CMeshRender.h>

// [ SCRIPT PART ]
#include "BossJugCombatMgrScript.h"
#include "BossJugScript.h"
#include "ColumnFlameScript.h"
#include "EnergyBallScript.h"
#include "HammerScript.h"
#include "PlayerScript.h"
#include "PaperBurnScript.h"

#define FLAME_COUNT 6
#define ENERGYBALL_COUNT 3

JugPhase_2::JugPhase_2()
	: CState(L"JUG_PHASE_2")
	, m_pCombatMgr(nullptr)
	, m_pBossFSM(nullptr)
	, m_pBossAnimator(nullptr)
	, m_fIdleTime(3.f)
	, m_fAttackTime(5.f)
	, m_fRotTimer(1.5f)
	, m_iRotCounter(0)
	, m_iPrevAttackPattern(0)
	, m_iAttackPattern(0)
	, m_bRot(false)
	, m_bAttackProceeding(false)
	, m_fDMG{80.f, 30.f, 0.f, 0.f}
{
}

JugPhase_2::JugPhase_2(const JugPhase_2& _origin)
	: CState(_origin.m_sStateType)
	, m_pCombatMgr(nullptr)
	, m_pBossFSM(nullptr)
	, m_pBossAnimator(nullptr)
	, m_fIdleTime(3.f)
	, m_iPrevAttackPattern(0)
	, m_iAttackPattern(0)
	, m_bAttackProceeding(false)
	, m_bRot(false)
	, m_fAttackTime(_origin.m_fAttackTime)
	, m_fRotTimer(1.5f)
	, m_iRotCounter(0)
	, m_fDMG{80.f, 30.f, 0.f, 0.f}
{
}

JugPhase_2::~JugPhase_2()
{
}

void JugPhase_2::Init()
{
	m_pCombatMgr = GetOwner()->GetScript<BossJugCombatMgrScript>();
	assert(m_pCombatMgr);

	// 불기둥 생성
	if (m_vecColumnFlames.empty())
	{
		for (int i = 0; i < FLAME_COUNT; i++)
		{
			CGameObject* pFlame = new CGameObject;
			pFlame->SetName(L"FLAME_" + std::to_wstring(i));
			pFlame->AddComponent(new CTransform);
			pFlame->AddComponent(new CCollider3D{});
			pFlame->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
			pFlame->Collider3D()->SetOffsetScale(Vec3(60.f, 300.f, 60.f));
			pFlame->Collider3D()->SetOffsetPos(Vec3(0.f, -150.f, 0.f));

			pFlame->AddComponent(new ColumnFlameScript);
			ColumnFlameScript* pFlameScript = pFlame->GetScript<ColumnFlameScript>();
			pFlameScript->Init();
			pFlameScript->SetDamage(m_fDMG[1]);
			pFlameScript->SetColor(Vec4(0.f, 1.f, 1.f, 1.f));
			pFlameScript->SetFlameSpeed(1.5f);
			pFlameScript->SetRotSpeed(25.f);

			m_vecColumnFlames.push_back(pFlame);
			CSceneMgr::GetInst()->SpawnObject(pFlame, GAME::LAYER::MONSTER_NON_PARRING_ATTACK);
			pFlame->Deactivate();
		}
	}

	// 에너지볼 생성
	if (m_vecEnergyBalls.empty())
	{
		for (int i = 0; i < ENERGYBALL_COUNT; i++)
		{
			//에너지볼 생성
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

	// Hammer 충돌체 적용
	CGameObject* pHammerCollider = new CGameObject;
	pHammerCollider->SetName(L"HammerCollider");
	pHammerCollider->AddComponent(new CTransform);
	pHammerCollider->AddComponent(new CCollider3D);
	pHammerCollider->AddComponent(new HammerScript);
	pHammerCollider->Collider3D()->SetOffsetScale(Vec3(250.f, 500.f, 250.f));
	pHammerCollider->GetScript<HammerScript>()->SetHammerColliderOffsetPos(Vec3(0.f, 0.f, 200.f));
	m_pCombatMgr->GetHammer()->AddChild(pHammerCollider);
}

void JugPhase_2::Enter()
{
	m_pBossFSM = m_pCombatMgr->GetJug()->FSM();
	assert(m_pBossFSM);

	m_pBossAnimator = m_pCombatMgr->GetJug()->Animator3D();
	assert(m_pBossAnimator);

	// 현재 보스 상태를 JUG_SPAWNHAMMER로 바꾼다.
	m_pBossFSM->ChangeState(L"JUG_SPAWNHAMMER");
}

void JugPhase_2::Update()
{
	if (nullptr == m_pCombatMgr->GetPlayer())
		return;

	CState::Update();

	CState* pCurState = m_pBossFSM->GetCurState();

	// 만약 현재 상태가 SPAWNHAMMER이고 애니메이션이 종료된 상태라면 다시 IDLE 상태로 돌아온다
	if (L"JUG_SPAWNHAMMER" == pCurState->GetStateType())
	{
		if (ANIMATION_STATE::FINISH == m_pBossAnimator->GetCurAnim()->GetState())
			m_pBossFSM->ChangeState(L"JUG_HAMMER_IDLE");

		else
		{
			// 507 
			if (507 == m_pBossAnimator->GetCurAnim()->GetCurFrameIdx())
			{
				m_pCombatMgr->GetHammer()->Activate();

				CScript* pPaperburnScript = m_pCombatMgr->GetHammer()->GetScript<PaperBurnScript>();

				((PaperBurnScript*)pPaperburnScript)->On();
				((PaperBurnScript*)pPaperburnScript)->SetDir(-1.f);
				((PaperBurnScript*)pPaperburnScript)->SetStrength(1.f);
			}
		}
	}
	else
	{
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

	// 플래이어 방향으로 회전
	if (m_bRot)
	{
		RotTowardPlayer();
	}

	switch (m_iAttackPattern)
	{
	case 0:
		if (GAME::BOSS::JUG_HAMMER_IDLE == m_pBossFSM->GetCurState()->GetStateType())
			ChangePattern();
		break;
	case 1:
		Attack_1();
		break;
	case 2:
		Attack_2();
		break;
	case 3:
		Attack_3();
		break;
	}


	for (size_t i = 0; i < ENERGYBALL_COUNT; i++)
	{
		if (m_vecEnergyBalls[i]->GetScript<EnergyBallScript>()->IsFinish())
		{
			if (ENERGYBALL_MODE::ROTATION == m_vecEnergyBalls[i]->GetScript<EnergyBallScript>()->GetCurMode())
			{
				Vec3 vPlayerPos = m_pCombatMgr->GetPlayer()->Transform()->GetWorldPos();
				m_vecEnergyBalls[i]->GetScript<EnergyBallScript>()->SetSpeed(800.f);
				m_vecEnergyBalls[i]->GetScript<EnergyBallScript>()->
				                     Start(ENERGYBALL_MODE::MISSILE,
				                           vPlayerPos,
				                           2.5f);
			}
		}
	}
}

void JugPhase_2::Exit()
{
}

void JugPhase_2::LateUpdate()
{
	CState::LateUpdate();
}


void JugPhase_2::RotTowardPlayer()
{
	Vec3 vBossPos   = m_pCombatMgr->GetJug()->Transform()->GetWorldPos();
	Vec3 vPlayerPos = m_pCombatMgr->GetPlayer()->Transform()->GetWorldPos();

	Vec3 vBossFront = m_pCombatMgr->GetJug()->Transform()->GetWorldFrontDir();
	vBossFront *= -1;

	// 보스의 정면 벡터와 보스-플레이어 벡터 사이의 각을 구한다.
	Vec3 v1(vBossFront.x, 0.f, vBossFront.z);
	Vec3 v2(vPlayerPos.x - vBossPos.x, 0.f, vPlayerPos.z - vBossPos.z);

	v1.Normalize();
	v2.Normalize();

	float dot   = XMConvertToDegrees(v1.Dot(v2)); // 0 ~ 180
	Vec3  cross = v1.Cross(v2); // -1 ~ 1

	float fSpeed = 0;

	// 회전 방향을 찾는다
	if (0.f < cross.y)
		fSpeed = 10.f;
	else if (0.f > cross.y)
		fSpeed = -10.f;

	// 각이 5도 이상이면 플레이어 방향으로 지속적으로 회전하고 5도 이하이면 플레이를 바로 바라본다.
	if (fabsf(dot) > 5.f)
	{
		Vec3 vBossRot = m_pCombatMgr->GetJug()->Transform()->GetRelativeRotation();
		vBossRot.ToDegree();
		vBossRot.y += DT * fSpeed;
		vBossRot.ToRadian();
		m_pCombatMgr->GetJug()->Transform()->SetRelativeRotation(vBossRot);
	}
	else
	{
		Vec3 vBossRot = m_pCombatMgr->GetJug()->Transform()->GetRelativeRotation();
		vBossRot.ToDegree();
		vBossRot.y = dot * (cross.y ? -1.f : 1.f);
		vBossRot.ToRadian();
		m_pCombatMgr->GetJug()->Transform()->SetRelativeRotation(vBossRot);
	}
}

void JugPhase_2::ChangePattern()
{
	if (GetTimer() < m_fIdleTime)
		return;

	ResetTimer();

	std::random_device                 rd;
	std::mt19937                       gen(rd());
	std::uniform_int_distribution<int> dis(1, 3);

	m_iAttackPattern = dis(gen);

	if (m_iAttackPattern == m_iPrevAttackPattern)
	{
		++m_iAttackPattern;

		if (m_iAttackPattern > 3)
			m_iAttackPattern = 1;

		m_iPrevAttackPattern = 0;
	}
}

// 망치 공격
void JugPhase_2::Attack_1()
{
	if (!m_bAttackProceeding)
	{
		m_bAttackProceeding = true;
		m_bRot              = true;
		m_pBossFSM->ChangeState(GAME::BOSS::JUG_ATTACK_1);

		CResMgr::GetInst()->FindRes<CSound>(L"sound\\jug\\JuguWhooshHammer.wav")->Play(1, 0.5f, false);
	}
	else
	{
		static bool bDamage = false;

		if (GetTimer() > m_fAttackTime)
		{
			m_pBossFSM->ChangeState(GAME::BOSS::JUG_HAMMER_IDLE);
			m_bAttackProceeding  = false;
			m_iPrevAttackPattern = m_iAttackPattern;
			m_iAttackPattern     = 0;
			ResetTimer();
			bDamage = false;
			return;
		}

		// 공격 모션이 끝나면 회전하지 않는다.
		if (m_pBossAnimator->GetCurAnim()->IsFinish())
			m_bRot = false;

		if (GetTimer() > 1.5f)
		{
			if (!bDamage)
			{
				PlayerScript* pPlayerScript = m_pCombatMgr->GetPlayer()->GetScript<PlayerScript>();
				if (nullptr != pPlayerScript)
					pPlayerScript->Stat_Down(STAT_TYPE::HP, 0.25f);

				bDamage = true;
			}
		}
	}
}

// 화염 공격
void JugPhase_2::Attack_2()
{
	if (!m_bAttackProceeding)
	{
		m_bAttackProceeding = true;
		m_bRot              = true;
		m_pBossFSM->ChangeState(GAME::BOSS::JUG_ATTACK_0);
	}
	else
	{
		// 공격 종료
		if (GetTimer() > m_fAttackTime)
		{
			m_pBossFSM->ChangeState(GAME::BOSS::JUG_HAMMER_IDLE);
			m_bAttackProceeding  = false;
			m_iPrevAttackPattern = m_iAttackPattern;
			m_iAttackPattern     = 0;
			ResetTimer();

			return;
		}

		// 공격 모션이 끝나면 회전하지 않는다.
		if (m_pBossAnimator->GetCurAnim()->IsFinish())
			m_bRot = false;

		// 공격
		if (GetTimer() > 1.5f)
		{
			if (FLAME_COUNT == 0)
				return;

			static float fFlameTimer   = 0.f;
			static int   fFlameCounter = 0;

			if (fFlameTimer >= 0.6f)
			{
				Vec3 vPlayerPos = m_pCombatMgr->GetPlayer()->Transform()->GetWorldPos();

				if (vPlayerPos.y > 15)
					vPlayerPos.y = 15.f;

				vPlayerPos.y += 300.f;
				m_vecColumnFlames[fFlameCounter]->Transform()->SetRelativePos(vPlayerPos);
				m_vecColumnFlames[fFlameCounter]->Activate();

				fFlameTimer = 0.f;
				++fFlameCounter;
				if (FLAME_COUNT - 1 < fFlameCounter)
				{
					fFlameCounter = 0;
				}
			}
			else
				fFlameTimer += DT;
		}
	}
}

// 에너지볼 공격
void JugPhase_2::Attack_3()
{
	if (!m_bAttackProceeding)
	{
		m_bAttackProceeding = true;
		m_bRot              = true;
		m_pBossFSM->ChangeState(GAME::BOSS::JUG_ATTACK_0);

		CResMgr::GetInst()->FindRes<CSound>(L"sound\\jug\\JuguCircleBalls.wav")->Play(1, 0.5f, false);
	}
	else
	{
		if (GetTimer() > m_fAttackTime)
		{
			m_pBossFSM->ChangeState(GAME::BOSS::JUG_HAMMER_IDLE);
			m_bAttackProceeding  = false;
			m_iPrevAttackPattern = m_iAttackPattern;
			m_iAttackPattern     = 0;
			m_iRotCounter        = 0;
			m_fRotTimer          = 1.5f;
			ResetTimer();

			return;
		}

		// 공격 모션이 끝나면 회전하지 않는다.
		if (m_pBossAnimator->GetCurAnim()->IsFinish())
			m_bRot = false;

		if (m_fRotTimer <= 0.f)
		{
			if (m_iRotCounter >= ENERGYBALL_COUNT)
				return;

			m_vecEnergyBalls[m_iRotCounter]->GetScript<EnergyBallScript>()->SetRadius(300.f);
			m_vecEnergyBalls[m_iRotCounter]->GetScript<EnergyBallScript>()->SetSpeed(6.f);
			m_vecEnergyBalls[m_iRotCounter]->GetScript<EnergyBallScript>()->SetRotDir(ROT_DIR::VERTICAL);
			m_vecEnergyBalls[m_iRotCounter]->Activate();
			m_vecEnergyBalls[m_iRotCounter]->GetScript<EnergyBallScript>()->
			                                 Start(ENERGYBALL_MODE::ROTATION,
			                                       Vec3(0.f, 400.f, -200.f),
			                                       1.0f);

			++m_iRotCounter;
			m_fRotTimer = 0.5f;
		}
		else
		{
			m_fRotTimer -= DT;
		}
	}
}

void JugPhase_2::SetDamage(int _idx, float _dmg)
{
	// 공격 패턴 개수 이상의 인덱스를 입력하면 경고 
	if (3 < _idx)
		return;

	m_fDMG[_idx] = _dmg;
}
