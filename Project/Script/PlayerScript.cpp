#include "pch.h"
#include "PlayerScript.h"

//#include "CState.h" // .. 각 상태별 헤더들은 CState.h 에 정의했습니다. 

#include "CStateMgr.h"

#include "CObjKeyMgr.h"

#include "PlayerCamScript.h"

#include <Engine/CCollider3D.h>
#include <Engine/CTransform.h>
#include <Engine/CAnimation3D.h>
#include <Engine/CAnimator3D.h>
#include <Engine/CParticleSystem.h>
#include <Engine/CFSM.h>


#include "SwordTrailScript.h"

PlayerScript::PlayerScript()
	: CScript((int)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_pKeyMgr(nullptr)
	, m_pStateMgr(nullptr)
	, m_pStat(nullptr)
{
	SetName(L"PlayerScript");
}

PlayerScript::PlayerScript(const PlayerScript& _origin)
	: CScript((int)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_pKeyMgr(nullptr)
	, m_pCamera(nullptr)
	, m_pStateMgr(nullptr)
	, m_pStat(nullptr)
{
	SetName(L"PlayerScript");
}


PlayerScript::~PlayerScript()
{
	SAFE_DELETE(m_pKeyMgr);
	SAFE_DELETE(m_pStateMgr);
	SAFE_DELETE(m_pStat);
}

void PlayerScript::start()
{
	// 키 정보 관리 초기화
	if (nullptr == m_pKeyMgr)
	{
		m_pKeyMgr = new CObjKeyMgr;
		m_pKeyMgr->SetOwnerScript(this);
	}


	// 상태 정보 관리 초기화
	if (nullptr == m_pStateMgr)
	{
		m_pStateMgr = new CStateMgr;
		m_pStateMgr->InitState(GetOwner());
	}


	// 스탯 정보 관리 초기화 
	if (nullptr == m_pStat)
	{
		m_pStat = new CPlayerStat;
		m_pStat->SetOwnerScript(this);
		m_pStat->start();
	}


	// 카메라 초기화 - 기본적으로 외부에서 초기화한다. 
	if (nullptr == m_pCamera)
	{
		// 외부에서 초기화를 안해줬다면
		// CAMERA 레이어 / MainCamera 라는 Obj 를 찾는다. ( 지정되어야함 )
		CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(L"CAMERA");
		if (pLayer != nullptr)
		{
			vector<CGameObject*> vecObjs = pLayer->GetObjects();
			for (int i = 0; i < vecObjs.size(); ++i)
			{
				if (vecObjs[i]->GetName() == L"MainCamera")
				{
					m_pCamera = vecObjs[i];
					m_pStateMgr->SetCamera(m_pCamera); // 카메라 설정 

					break;
				}
			}
		}
	}

	if (m_pStateMgr->GetCamera() == nullptr)
	{
		if (m_pCamera)
			m_pStateMgr->SetCamera(m_pCamera);
	}

	// 무기 충돌체 초기화
	if (m_pSwordCollider == nullptr)
	{
		m_pSwordCollider = GetChildObj(GetOwner(), L"Sword_Bone_Collider");

		if (!m_pSwordCollider)
		{
			m_pSwordCollider = new CGameObject;
			m_pSwordCollider->SetName(L"Sword_Bone_Collider");

			m_pSwordCollider->AddComponent(new CTransform);
			m_pSwordCollider->AddComponent(new CCollider3D);

			m_pSwordCollider->Collider3D()->SetOffsetScale(Vec3(100.f, 100.f, 100.f));

			CSceneMgr::GetInst()->GetCurScene()->AddObject(m_pSwordCollider, L"PLAYER");
			GetOwner()->AddChild(m_pSwordCollider);
		}
	}

	// 무기 트레일 초기화  
	if (m_pSword_Trail == nullptr)
	{
		m_pSword_Trail = GetChildObj(GetOwner(), L"Sword_Trail");
		if (m_pSword_Trail)
		{
			m_pSword_Trail->Transform()->SetIgnoreParent(true);		// 부모의 모든 행렬을 적용받지 않음 
			m_pSword_Trail->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));
		}
	}

	// 빛 초기화
	if (!m_pDirectionalLight)
	{
		CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(L"BG_OBJ");
		if (pLayer)
			m_pDirectionalLight = pLayer->FindRootObject(L"Directional Light");
	}
}

void PlayerScript::update()
{

	// 1. 키 상태 업데이트 
	m_pKeyMgr->update();
	tKey_Zip tCurKeyInfo  = m_pKeyMgr->GetCurKeyInfo();
	tKey_Zip tPrevKeyInfo = m_pKeyMgr->GetPrevKeyInfo();

	// 2. 플레이어 상태 업데이트 
	m_pStateMgr->SetCurKeyInfo(tCurKeyInfo);
	m_pStateMgr->SetPrevKeyInfo(tPrevKeyInfo);
	m_pStateMgr->Update();

	// 3. 플레이어 스탯 업데이트 
	m_pStat->update();

	// 4. 카메라 상태 업데이트 
	UpdateCamera();

	// 5. 무기 충돌체 업데이트 
	UpdateSwordCollider();

	// 6. 빛 업데이트 
	UpdateDirectionalLight();

	// Die 상태 체크 
	if (m_pStat->GetStat().fHp <= 0.f)
	{
		if(m_pStateMgr->GetCurstate() != L"DIE")
			GetOwner()->FSM()->ChangeState(L"DIE");
	}


}

void PlayerScript::lateupdate()
{
	m_pKeyMgr->lateupdate();

	m_pStateMgr->LateUpdate();

	m_pStat->lateupdate();
}

void PlayerScript::UpdateCamera()
{
	if (m_pCamera == nullptr)
		return;

	PlayerCamScript* pCamScript = (PlayerCamScript*)m_pCamera->GetScriptByName(L"PlayerCamScript");
	if (pCamScript)
	{
		// 카메라 스크립트에 플레이어의 위치 정보를 넘긴다 .  
		pCamScript->SetTargetPos(GetOwner()->Transform()->GetRelativePos());
	}
}

void PlayerScript::UpdateSwordCollider()
{
	if (!m_pSwordCollider)
		return;


	CAnimator3D* pAnimator3D = GetOwner()->Animator3D();
	if (pAnimator3D)
	{
		CAnimation3D* pAnim = pAnimator3D->GetCurAnim();
		if (pAnim)
		{
			int    SocketIdx   = 0;
			Matrix SocketMat   = pAnimator3D->GetSocket(SocketIdx);
			Vec3   SocketRot   = DecomposeRotMat(SocketMat);
			Vec3   SocketTrans = Vec3(SocketMat._41, SocketMat._42, SocketMat._43);

			// 회전 적용 
			m_pSwordCollider->Transform()->SetRelativeRotation(SocketRot);

			Vec3 vScale       = m_pSwordCollider->Collider3D()->GetOffsetScale();
			Vec3 vForward     = m_pSwordCollider->Transform()->GetWorldFrontDir();
			Vec3 vOffsetTrans = vForward * (vScale.z / 2.f);

			// 위치 적용 
			m_pSwordCollider->Transform()->SetRelativePos(SocketTrans); // 메인 위치
			m_pSwordCollider->Collider3D()->SetOffsetPos(vOffsetTrans); // 임의로 위치 조정  
		}
	}
}

void PlayerScript::UpdateDirectionalLight()
{

	if (!m_pDirectionalLight)
		return;

	Vec3 vCamPos      = m_pCamera->Transform()->GetRelativePos();
	Vec3 vCamForward  = m_pCamera->Transform()->GetWorldFrontDir();
	Vec3 vCamBackward = -1 * vCamForward;

	Vec3 vLightPos = Vec3(vCamPos.x + vCamBackward.x * 500.f,
	                      vCamPos.y + 500.f,
	                      vCamPos.z + vCamBackward.z * 500.f);

	m_pDirectionalLight->Transform()->SetRelativePos(vLightPos);

}

CGameObject* PlayerScript::GetChildObj(CGameObject* _parent, wstring _name)
{
	vector<CGameObject*> vecChild = _parent->GetChild();
	for (int i = 0; i < vecChild.size(); ++i)
	{
		if (vecChild[i]->GetName() == _name)
		{
			return vecChild[i];
		}
		else
		{
			CGameObject* pObj = GetChildObj(vecChild[i], _name);
			if (pObj != nullptr)
				return pObj;
		}
	}

	return nullptr;
}

void PlayerScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	// 상태 바뀌는거 
	// 패링 방향 바꾸는거 
	if (!_OtherObject)
		return;

	// 충돌체 레이어 인덱스 
	int           iLayerIdx       = _OtherObject->GetLayerIndex();
	const wstring _OtherLayerName = CSceneMgr::GetInst()->GetCurScene()->GetLayerNameFromIdx(iLayerIdx);


	// [ Monster Attack Check ]
	// 1. Non _ Parrying Attack Check
	GAME::LAYER::MONSTER_NON_PARRING_ATTACK;


	// 2. Parrying Attack Check
	GAME::LAYER::MONSTER_PARRING_ATTACK;
	// 방향 : 플레이어가 바라보는 방향으로 이동방향을 바꾼다.
	Vec3 vChangedDir = GetOwner()->Transform()->GetWorldFrontDir();

}

void PlayerScript::OnCollision(CGameObject* _OtherObject)
{


}

void PlayerScript::OnCollisionExit(CGameObject* _OtherObject)
{


}


void PlayerScript::SaveToScene(FILE* _pFile)
{
}

void PlayerScript::LoadFromScene(FILE* _pFile)
{
}


void PlayerScript::Stat_Up(STAT_TYPE _eType, float _fPercent)
{
	tSTAT MaxStat = m_pStat->GetMaxStat();

	switch (_eType)
	{
	case STAT_TYPE::HP:
		{
			float fHP = MaxStat.fHp * _fPercent;
			m_pStat->AddHp(fHP);
		}
		
		break;
	case STAT_TYPE::STAMINA:
		{
			float fStamina = MaxStat.fStamina * _fPercent;
			m_pStat->AddStamina(fStamina);
		}
		
		break;
	case STAT_TYPE::ETHER:
		{
			float fEther = MaxStat.fEther * _fPercent;
			m_pStat->AddEther(fEther);
		}
		
		break;
	case STAT_TYPE::ATTACK:
		{
			float fAttack = MaxStat.fEther * _fPercent;
			m_pStat->AddAttack(fAttack);
		}
		
		break;
	case STAT_TYPE::ARMOR:
		{
			float fArmor = MaxStat.fArmor * _fPercent;
			m_pStat->AddArmor(fArmor);
		}
		
		break;
	case STAT_TYPE::SPEED:
		{
			float fSpeed = MaxStat.fSpeed * _fPercent;
			m_pStat->AddSpeed(fSpeed);
		}
		
		break;
	}
}

void PlayerScript::Stat_Down(STAT_TYPE _eType, float _fPercent)
{
	tSTAT MaxStat = m_pStat->GetMaxStat();

	switch (_eType)
	{
	case STAT_TYPE::HP:
		{
		if (m_pStateMgr->GetCurstate() == L"SPRINT" ||
			m_pStateMgr->GetCurstate() == L"DIE")
				break;

			float fHP = MaxStat.fHp * _fPercent;
			m_pStat->SubHp(fHP);
			GetOwner()->FSM()->ChangeState(L"HIT");

		}
		
		break;
	case STAT_TYPE::STAMINA:
		{
			float fStamina = MaxStat.fStamina * _fPercent;
			m_pStat->SubStamina(fStamina);
		}
		
		break;
	case STAT_TYPE::ETHER:
		{
			float fEther = MaxStat.fEther * _fPercent;
			m_pStat->SubEther(fEther);
		}
		
		break;
	case STAT_TYPE::ATTACK:
		{
			float fAttack = MaxStat.fEther * _fPercent;
			m_pStat->SubAttack(fAttack);
		}
		
		break;
	case STAT_TYPE::ARMOR:
		{
			float fArmor = MaxStat.fArmor * _fPercent;
			m_pStat->SubArmor(fArmor);
		}
		
		break;
	case STAT_TYPE::SPEED:
		{
			float fSpeed = MaxStat.fSpeed * _fPercent;
			m_pStat->SubSpeed(fSpeed);
		}
		
		break;
	}
}
