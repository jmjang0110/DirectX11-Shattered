#include "pch.h"
#include "CToolObjMgr.h"

#include <Engine/CTransform.h>
#include <Engine/CRenderMgr.h>

#include "CGameObjectEx.h"
#include "CCameraEx.h"
#include "CEditorCamScript.h"
#include "CGridScript.h"


CToolObjMgr::CToolObjMgr() = default;

CToolObjMgr::~CToolObjMgr()
{
	Safe_Del_Vec(m_vecObj);
}

void CToolObjMgr::init()
{
	// Editor Camera
	CGameObjectEx* pToolObj = new CGameObjectEx;
	pToolObj->SetName(L"EditorCamera");
	pToolObj->AddComponent(new CTransform);
	pToolObj->AddComponent(new CCameraEx);
	pToolObj->AddComponent(new CEditorCamScript);
	pToolObj->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pToolObj->Camera()->SetFar(100000);
	pToolObj->Camera()->CheckLayerMaskAll();
	m_vecObj.push_back(pToolObj);

	CRenderMgr::GetInst()->RegisterEditorCamera(pToolObj->Camera());
	LOG_INFO("EDITOR CAMERA REGISTERED");

	// Grid Object
	CGameObjectEx* pGridObj = new CGameObjectEx;
	pGridObj->SetName(L"Grid");
	pGridObj->AddComponent(new CTransform);
	pGridObj->AddComponent(new CMeshRender);
	pGridObj->AddComponent(new CGridScript);
	pGridObj->Transform()->SetRelativeScale(Vec3(100000.f, 100000.f, 1.f));
	pGridObj->Transform()->SetRelativeRotation(Vec3(XM_PI / 2.f, 0.f, 0.f));
	pGridObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pGridObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\GridMtrl.mtrl"), 0);
	pGridObj->GetScript<CGridScript>()->SetToolCamera(pToolObj);
	pGridObj->GetScript<CGridScript>()->SetGridColor(Vec3(0.2f, 0.2f, 0.7f));
	m_vecObj.push_back(pGridObj);
}

void CToolObjMgr::progress()
{
	if (CSceneMgr::GetInst()->GetCurScene()->GetSceneState() == SCENE_STATE::PLAY)
	{
		return;
	}

	for (const auto& pToolObject : m_vecObj)
	{
		pToolObject->update();
	}

	for (const auto& pToolObject : m_vecObj)
	{
		pToolObject->lateupdate();
	}

	for (const auto& pToolObject : m_vecObj)
	{
		pToolObject->finalupdate();
	}

	for (const auto& pToolObject : m_vecObj)
	{
		if (nullptr == pToolObject->GetRenderComponent()
		    || nullptr == pToolObject->GetRenderComponent()->GetMesh()
		    || nullptr == pToolObject->GetRenderComponent()->GetMaterial(0)
		    || nullptr == pToolObject->GetRenderComponent()->GetMaterial(0)->GetShader())
		{
			continue;
		}

		pToolObject->render();
	}
}
