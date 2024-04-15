#include "pch.h"
#include "TrailScript.h"


#include "DestroyScript.h"


#include <Engine/CStructuredBuffer.h>
#include <Engine/CAnimator3D.h>
#include <Engine/CAnimation3D.h>
#include <Engine/CGameObject.h>
#include <Engine/CMeshData.h>
#include <Engine/CTransform.h>



TrailScript::TrailScript()
	: CScript((int)SCRIPT_TYPE::TRAILSCRIPT)
	, m_fTimer(0.f)
	, m_fCreateTerm(0.4f)
	, m_fDestroyTime(1.5f)
	, m_bCreateTrail(true)
	, m_pTrail(nullptr)
	, m_iTrailMaxCnt(15)
	, m_bPlayTrail(false)


{
	SetName(L"TrailScript");

}

TrailScript::TrailScript(const TrailScript& _origin)
	: CScript((int)SCRIPT_TYPE::TRAILSCRIPT)
	, m_fTimer(0.f)
	, m_fCreateTerm(0.4f)
	, m_fDestroyTime(3.f)
	, m_bCreateTrail(true)
	, m_pTrail(nullptr)
	, m_iTrailMaxCnt(15)
	, m_bPlayTrail(false)
{
	SetName(L"TrailScript");


}
TrailScript::~TrailScript()
{
	SAFE_DELETE(m_pTrail);
	//Safe_Del_Vec(m_vecTrail); -> Scene 에서 삭제시킬 것임 


}



int TrailScript::GetTrailFrameIdx()
{
	int frameIdx = 0;

	CAnimator3D* pAnimator3D = GetOwner()->Animator3D();
	if (pAnimator3D != nullptr)
	{
		CAnimation3D* pAnim = pAnimator3D->GetCurAnim();
		if (pAnim != nullptr)
		{
			frameIdx = pAnim->GetCurFrameIdx();

		}
	}


	return frameIdx;
}



void TrailScript::start()
{
	m_fTimer = 0.f;

	if (nullptr == m_pTrail)
	{
		SetTrailObj();
	}

	if (m_pCam == nullptr)
	{
		CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(L"CAMERA");
		if (pLayer != nullptr)
		{
			vector<CGameObject*> vecObjs = pLayer->GetObjects();
			for (int i = 0; i < vecObjs.size(); ++i)
			{
				if (vecObjs[i]->GetName() == L"MainCamera")
				{
					m_pCam = vecObjs[i];
					break;
				}
			}
		}
	}

	// 미리 만들어 놓기 5개 정도 
	// activate / Deactivate  - bool;
	//TODO::이거 하면 작업 목록에서 볼 수 있ㅇ므
	for (int i = 0; i < m_iTrailMaxCnt; ++i)
	{
		DestroyScript* pDestroyScript = new DestroyScript;
		pDestroyScript->SetDeadLineTime(m_fDestroyTime);
		pDestroyScript->SetDestroyMode(DESTROY_MODE::ACTIVE);


		// 잔상 오브젝트 미리 저장  
		CGameObject* pGameObj = CloneTrailObj();

		pGameObj->AddComponent(pDestroyScript);
		CSceneMgr::GetInst()->GetCurScene()->AddObject(pGameObj, 2);
		pGameObj->Deactivate();

		tTrail trail;
		trail.pTrailObj = pGameObj;
		trail.fTimer = 0.f;

		m_vecTrail.push_back(trail);

	}


}

void TrailScript::update()
{
	m_fTimer += DT;

	if (m_bPlayTrail == true)
	{
		Vec3 vCamPos = m_pCam->Transform()->GetRelativePos();
		Vec4 vCamPos_2 = Vec4(vCamPos, 1.f);

		GetOwner()->MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::VEC4_0, &vCamPos_2);
		GetOwner()->MeshRender()->GetMaterial(0)->UpdateData();

		UpdateActiveTrail();

		// 잔상 오브젝트를 생성한다.
		if (m_fTimer >= m_fCreateTerm)
		{
			TimerReset();
			if (m_bCreateTrail)
			{
				ActivateTrail();
				DeactivateTrail();

			}
		}
	}
	else
	{
		DeactivateTrail();

	}
}
void TrailScript::UpdateActiveTrail()
{
	for (int i = 0; i < m_vecTrail.size(); ++i)
	{
		if (m_vecTrail[i].pTrailObj->IsActive())
		{
			m_vecTrail[i].fTimer += DT;
		}
	}

}
void TrailScript::lateupdate()
{


}

void TrailScript::OnCollisionEnter(CGameObject* _OtherObject)
{
}

void TrailScript::OnCollision(CGameObject* _OtherObject)
{
}

void TrailScript::OnCollisionExit(CGameObject* _OtherObject)
{
}

void TrailScript::SaveToScene(FILE* _pFile)
{
}

void TrailScript::LoadFromScene(FILE* _pFile)
{
}

void TrailScript::SetOriginObject(CGameObject* _Origin)
{
	m_pOriginObj = _Origin;

}

void TrailScript::SetTrailMeshData(wstring _strKey)
{
	m_pOribinObj_MeshData = CResMgr::GetInst()->Load<CMeshData>(_strKey, _strKey);

}

void TrailScript::CreateTrail(int _FrameIdx)
{



}

bool TrailScript::DeactivateTrail()
{


	for (int i = 0; i < m_vecTrail.size(); ++i)
	{
		if (m_vecTrail[i].fTimer >= m_fDestroyTime)
		{
			bool bActivate = m_vecTrail[i].pTrailObj->IsActive();
			if (bActivate)
			{

				m_vecTrail[i].pTrailObj->Deactivate();
				m_vecTrail[i].fTimer = 0.f;

				// 사라지면 바로 생성될 수 있게
				m_fTimer = m_fCreateTerm;
			}
		}
	}



	return true;

}


bool TrailScript::ActivateTrail()
{

	for (int i = 0; i < m_vecTrail.size(); ++i)
	{
		if (m_vecTrail[i].pTrailObj->IsActive() == false)
		{
			m_vecTrail[i].pTrailObj->Activate();
			DestroyScript* pDestroy = (DestroyScript*)m_vecTrail[i].pTrailObj->GetScriptByName(L"DestroyScript");
			pDestroy->ResetTimer();

			CGameObject* pGameObject = m_vecTrail[i].pTrailObj;
			pGameObject->Transform()->SetRelativePos(m_pOriginObj->Transform()->GetRelativePos());
			pGameObject->Transform()->SetRelativeRotation(m_pOriginObj->Transform()->GetRelativeRotation());

			int FrameIdx = GetTrailFrameIdx();
			CAnimator3D* pAnimator = pGameObject->Animator3D();
			CAnimation3D* pAnim = pAnimator->FindAnim(L"TrailMotion");
			pAnim->SetStartFrameIdx(FrameIdx);
			pAnim->SetEndFrameIdx(FrameIdx);
			pAnimator->Play(L"TrailMotion", false);
			pAnimator->SetLerpTime(0.f);

			break;

		}
	}

	return true;

}

CGameObject* TrailScript::SetTrailObj()
{
	if (m_pTrail != nullptr)
		return nullptr;

	m_pOriginObj = GetOwner();

	wstring _strName = GetOwner()->GetName();
	_strName += L"_Trail";
	CGameObject* pObj = new CGameObject;
	pObj->SetName(_strName);

	// COMPONENT 세팅 
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);

	// MESHRENDER
	if (nullptr == m_pOribinObj_MeshData)
		SetTrailMeshData(L"meshdata\\player_sword0.mdat");

	pObj->MeshRender()->SetMesh(m_pOribinObj_MeshData->GetMesh());
	pObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\TrailMtrl.mtrl"), 0);

	// ANIMATION
	auto pAnimator = new CAnimator3D;
	pAnimator->SetBones(m_pOribinObj_MeshData->GetMesh()->GetBones());
	pAnimator->SetAnimClip(m_pOribinObj_MeshData->GetMesh()->GetAnimClip());
	pObj->AddComponent(pAnimator);

	m_pTrail = pObj;

	return pObj;

}

CGameObject* TrailScript::CloneTrailObj()
{
	CGameObject* pGameObject = CResMgr::GetInst()->Load<CPrefab>(L"prefab\\Trail.pref",
		L"prefab\\Trail.pref")->Instantiate();

	CAnimator3D* pAnimator = pGameObject->Animator3D();
	pAnimator->CreateAnimByFrame(L"TrailMotion", 0, 0, 0);

	return pGameObject;

	/*

	pGameObject->Transform()->SetRelativePos(m_pOriginObj->Transform()->GetRelativePos());
	pGameObject->Transform()->SetRelativeRotation(m_pOriginObj->Transform()->GetRelativeRotation());

	// TRAILMOTION
	int frameIdx = GetTrailFrameIdx();
	CAnimator3D* pAnimator = pGameObject->Animator3D();
	pAnimator->CreateAnimByFrame(L"TrailMotion", 0, frameIdx, frameIdx);
	pAnimator->Play(L"TrailMotion", false);
	pAnimator->SetLerpTime(0.f);

	// DESTROY SCRIPT
	DestroyScript* pDestroyScript = new DestroyScript;
	pDestroyScript->SetDeadLineTime(m_fDestroyTime);
	pDestroyScript->SetDestroyMode(DESTROY_MODE::TIMER);
	pGameObject->AddComponent(pDestroyScript);
	*/



	//CSceneMgr::GetInst()->GetCurScene()->AddObject(pGameObject, 2);


}
