#include "pch.h"
#include "InspectorUI.h"

#include <Engine/CKeyMgr.h>
#include <Engine/CScript.h>
#include <Engine/CGameObject.h>

#include "CImGuiMgr.h"
#include "SceneOutliner.h"

// Component UI
#include "TransformUI.h"
#include "MeshRenderUI.h"
#include "CameraUI.h"
#include "ScriptUI.h"
#include "MaterialUI.h"
#include "TextureUI.h"
#include "ParticleSystemUI.h"
#include "DecalUI.h"
#include "Animator3DUI.h"
#include "Collider3DUI.h"
#include "GameButtonUI.h"
#include "GameImageUI.h"
#include "GamePanelUI.h"
#include "GameProgressBarUI.h"
#include "GameTextUI.h"
#include "NaviMapUI.h"
#include "NaviAgentUI.h"
#include "BoundingBoxUI.h"

// etc UI
#include "ListUI.h"
#include "MaterialUI.h"
#include "TextureUI.h"
#include "IconsFontAwesome5.h"

// Engine > COMPONENT-TYPE
#include <Engine/CTransform.h>
#include <Engine/CCamera.h>
#include <Engine/CCollider2D.h>
#include <Engine/CAnimator2D.h>
#include <Engine/CMeshRender.h>
#include <Engine/CTileMap.h>
#include <Engine/CParticleSystem.h>
#include <Engine/CCollisionMgr.h>
#include <Engine/CScript.h>
#include <Engine/CLight2D.h>
#include <Engine/CLandScape.h>
#include <Engine/CSkyBox.h>
#include <Engine/CLight3D.h>
#include <Engine/CDecal.h>
#include <Engine/CAnimator3D.h>
#include <Engine/CUIButton.h>
#include <Engine/CUIImage.h>
#include <Engine/CUIPanel.h>
#include <Engine/CUIProgressBar.h>
#include <Engine/CUIText.h>
#include <Engine/CCollider3D.h>
#include <Engine/CNaviAgent.h>
#include <Engine/CNaviMap.h>
#include <Engine/CBoundingBox.h>


// Engine > etc
#include <Engine/CMaterial.h>
#include <Script/CScriptMgr.h>


InspectorUI::InspectorUI()
	: UI("Inspector")
	, m_pTargetObject(nullptr)
	, m_pTargetRes(nullptr)
	, m_arrComUI{}
	, m_arrResUI{}
{
	// ComponentUI 생성   
	ComponentUI* pComUI = nullptr;

	pComUI = new TransformUI;
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::TRANSFORM] = pComUI;

	pComUI = new Collider3DUI{};
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::COLLIDER3D] = pComUI;

	pComUI = new MeshRenderUI;
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER] = pComUI;

	pComUI = new CameraUI;
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::CAMERA] = pComUI;

	pComUI = new ParticleSystemUI{};
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::PARTICLESYSTEM] = pComUI;

	pComUI = new DecalUI{};
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::DECAL] = pComUI;

	pComUI = new Animator3DUI;
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::ANIMATOR3D] = pComUI;

	pComUI = new GameButtonUI{};
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::UIBUTTON] = pComUI;

	pComUI = new GameImageUI{};
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::UIIMAGE] = pComUI;

	pComUI = new GamePanelUI{};
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::UIPANEL] = pComUI;

	pComUI = new GameTextUI{};
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::UITEXT] = pComUI;

	pComUI = new GameProgressBarUI{};
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::UIPROGRESSBAR] = pComUI;

	pComUI = new NaviMapUI{};
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::NAVIMAP] = pComUI;

	pComUI = new NaviAgentUI{};
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::NAVIAGENT] = pComUI;


	pComUI = new BoundingBoxUI{};
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::BOUNDINGBOX] = pComUI;



	// ==============
	// ResInfoUI 생성
	// ==============
	ResInfoUI* pResInfoUI = nullptr;

	// MaterialUI
	pResInfoUI = new MaterialUI;
	AddChild(pResInfoUI);
	m_arrResUI[(UINT)RES_TYPE::MATERIAL] = pResInfoUI;

	// TextureUI
	pResInfoUI = new TextureUI;
	AddChild(pResInfoUI);
	m_arrResUI[(UINT)RES_TYPE::TEXTURE] = pResInfoUI;
}

InspectorUI::~InspectorUI() {}


void InspectorUI::update()
{
	//if (KEY_TAP(KEY::I))
	//{
	//	if (IsActive())
	//		Deactivate();
	//	else
	//		Activate();
	//}
}

void InspectorUI::render_update()
{
	RenderButton();
}


void InspectorUI::SetTargetObject(CGameObject* _pTarget)
{
	m_pTargetObject = _pTarget;

	for (int i = 0; i < (int)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrComUI[i])
		{
			// Object 가 nullptr 인 경우
			if (nullptr == m_pTargetObject)
				m_arrComUI[i]->Deactivate();
			else if (m_pTargetObject->GetComponent((COMPONENT_TYPE)i))
			{
				m_arrComUI[i]->Activate();
				m_arrComUI[i]->SetTargetObject(m_pTargetObject);
			}
			else
			{
				m_arrComUI[i]->Deactivate();
			}
		}
	}

	// 오브젝트가 null 이면
	if (nullptr == m_pTargetObject)
	{
		// 모든 스크립트UI 비활성화
		for (int i = 0; i < m_vecScriptUI.size(); ++i)
		{
			m_vecScriptUI[i]->Deactivate();
		}
	}
	else
	{
		const vector<CScript*>& vecScripts = m_pTargetObject->GetScripts();
		ScriptUI*               pScriptUI  = nullptr;

		for (size_t i = 0; i < vecScripts.size(); ++i)
		{
			if (m_vecScriptUI.size() <= i)
				pScriptUI = AddScriptUI();
			else
				pScriptUI = m_vecScriptUI[i];

			// 삭제시키기 위해서 title로 이름 저장 
			wstring ScriptName = CScriptMgr::GetScriptName(vecScripts[i]);
			pScriptUI->SetTitle(ToString(ScriptName));

			pScriptUI->SetTargetObject(m_pTargetObject);
			pScriptUI->SetTargetScript(vecScripts[i]);
			pScriptUI->Activate();
		}

		// ScriptUI 가 더 많이 있을때
		if (vecScripts.size() < m_vecScriptUI.size())
		{
			// 대응하는 UI 를 제외한 나머지 ScriptUI 들을 비활성화 한다.
			for (size_t i = vecScripts.size(); i < m_vecScriptUI.size(); ++i)
			{
				m_vecScriptUI[i]->Deactivate();
			}
		}
	}


	// ResInfoUI 비활성화
	for (int i = 0; i < (int)RES_TYPE::END; ++i)
	{
		if (nullptr != m_arrResUI[i] && m_arrResUI[i]->IsActive())
			m_arrResUI[i]->Deactivate();
	}
}

void InspectorUI::SetTargetResource(CRes* _pTargetRes)
{
	// ComponentUI 전부 비활성화
	for (int i = 0; i < (int)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrComUI[i] && m_arrComUI[i]->IsActive())
			m_arrComUI[i]->Deactivate();
	}

	// ScriptUI 전부 비활성화
	for (size_t i = 0; i < m_vecScriptUI.size(); ++i)
	{
		m_vecScriptUI[i]->Deactivate();
	}

	// 활성화 시킬 RES_TYPE 을 알아냄
	RES_TYPE type = _pTargetRes->GetResType();

	// 해당 리소스 ResInfoUI 활성화
	for (int i = 0; i < (int)RES_TYPE::END; ++i)
	{
		if (nullptr != m_arrResUI[(int)i])
		{
			if (i == (int)type)
			{
				m_arrResUI[i]->Activate();
				m_arrResUI[i]->SetTargetRes(_pTargetRes);
			}
			else
				m_arrResUI[i]->Deactivate();
		}
	}
}

ScriptUI* InspectorUI::AddScriptUI()
{
	ScriptUI* pScriptUI = new ScriptUI;
	pScriptUI->Deactivate();
	AddChild(pScriptUI);

	m_vecScriptUI.push_back(pScriptUI);

	return pScriptUI;
}

void InspectorUI::RenderButton()
{
	if (m_pTargetObject == nullptr)
		return;

	ImVec2 ToolBoxSize = ImVec2(0.f, 60.f);
	ImGui::BeginChild("GameObject_Component_Button", ToolBoxSize, false, ImGuiWindowFlags_AlwaysAutoResize);
	static bool bChangeName = false;

	if (bChangeName == false)
	{
		// [ CHANGE NAME BUTTON ]
		ImGui::PushID(0);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(80.f / 255.f, 80.f / 255.f, 80.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(80.f / 255.f, 80.f / 255.f, 80.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(80.f / 255.f, 80.f / 255.f, 80.f / 255.f, 1.f));
		wstring wstrObjName = m_pTargetObject->GetName();
		string  strObjName  = ToString(wstrObjName);
		ImGui::Button(strObjName.c_str());
		ImGui::PopStyleColor(3);
		ImGui::PopID();
	}


	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(80.f / 255.f, 80.f / 255.f, 80.f / 255.f, 1.f));
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_PENCIL))
		bChangeName = !bChangeName;

	if (bChangeName)
	{
		ImGui::PushItemWidth(200.f);
		ImGui::SameLine();
		static char buf[512];
		ImGui::InputText("##name", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_None);
		ImGui::PopItemWidth();

		// COMPLETE
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_O))
		{
			if (buf[0] != NULL)
			{
				string  strName  = buf;
				wstring wstrName = ToWString(strName);
				m_pTargetObject->SetName(wstrName);

				// CImGuiMgr 에 Delegate 등록 
				tUIDelegate tDeleteCom{};
				tDeleteCom.dwParam = (DWORD_PTR)nullptr;
				tDeleteCom.pFunc   = (PARAM_1)&SceneOutliner::ResetTreeUI;
				tDeleteCom.pInst   = CImGuiMgr::GetInst()->FindUI("SceneOutliner");
				CImGuiMgr::GetInst()->AddDelegate(tDeleteCom);

				char empty[512] = {NULL,};
				strcpy_s(buf, empty);
				bChangeName = false;
			}
		}
		// CANCEL
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_XMARK))
		{
			char empty[512] = {NULL,};
			strcpy_s(buf, empty);
			bChangeName = false;
		}
	}
	ImGui::PopStyleColor(1);

	// [ DELETE BUTTON ]
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(250.f / 255.f, 30.f / 255.f, 30.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(250.f / 255.f, 30.f / 255.f, 30.f / 255.f, 1.f));
	ImGui::SameLine(290.f);
	if (ImGui::Button(ICON_FA_TRASH))
		ImGui::OpenPopup(u8"OBJECT 삭제 경고창");
	ImGui::PopStyleColor(2);

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(u8"OBJECT 삭제 경고창", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		wstring wstrObjName = m_pTargetObject->GetName();
		string  strObjName  = ToString(wstrObjName);
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.3f, 1.f), u8"[ %s ] \n\nOBJECT를 정말로 삭제하시겠습니까?\n\n", strObjName.c_str());
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			// [ ENGINE ]
			CSceneMgr::GetInst()->DeleteObject(m_pTargetObject);

			// [ CLIENT ]
			tUIDelegate tInfo{};
			tInfo.dwParam = (DWORD_PTR)nullptr;
			tInfo.pFunc   = (PARAM_1)&InspectorUI::SetTargetObject;
			tInfo.pInst   = CImGuiMgr::GetInst()->FindUI("Inspector");
			CImGuiMgr::GetInst()->AddDelegate(tInfo);

			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

	// [ ADD COMPONENT BUTTON ]
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(137.f / 255.f, 170.f / 255.f, 74.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(153.f / 255.f, 204.f / 255.f, 102.f / 255.f, 1.f));
	string ButtonName = ICON_FA_PLUS;
	ButtonName += " Add Component";
	if (ImGui::Button(ButtonName.c_str(), ImVec2(150.f, 30.f)))
	{
		ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->Clear();
		pListUI->SetTitle("Component List");

		for (int i = 0; i < (int)COMPONENT_TYPE::END; ++i)
		{
			if (nullptr == m_pTargetObject->GetComponent((COMPONENT_TYPE)i))
			{
				pListUI->AddList(ToString((COMPONENT_TYPE)i));
			}
		}
		pListUI->Activate();
		pListUI->SetDBCEvent(this, (DBCLKED)&InspectorUI::AddComponent);
	}
	ImGui::PopStyleColor(2);

	ImGui::SameLine();

	// [ ADD SCRIPT BUTTON ]
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(51.f / 255.f, 153.f / 255.f, 255.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f / 255.f, 102.f / 255.f, 203.f / 255.f, 1.f));

	ButtonName = ICON_FA_GEARS;
	ButtonName += " Add Script";
	if (ImGui::Button(ButtonName.c_str(), ImVec2(150.f, 30.f)))
	{
		// Todo : AddScript 
		// ListUI 활성화
		ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->Clear();
		pListUI->SetTitle("Script List");

		vector<wstring> vecScriptInfo;
		CScriptMgr::GetScriptInfo(vecScriptInfo);

		for (int i = 0; i < vecScriptInfo.size(); ++i)
		{
			if (nullptr == m_pTargetObject->GetScriptByName(vecScriptInfo[i]))
				pListUI->AddList(ToString(vecScriptInfo[i]));
		}

		pListUI->Activate();
		pListUI->SetDBCEvent(this, (DBCLKED)&InspectorUI::AddScript);
	}
	ImGui::PopStyleColor(2);

	ImGui::EndChild();
}


// _param : string ( Component Type Name )
void InspectorUI::AddComponent(DWORD_PTR _param)
{
	if (nullptr == m_pTargetObject)
		return;

	string strComType = (char*)_param;
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (strComType == ToString((COMPONENT_TYPE(i))))
		{
			COMPONENT_TYPE eComType = (COMPONENT_TYPE)i;
			switch (eComType)
			{
			case COMPONENT_TYPE::TRANSFORM:
				{
					m_pTargetObject->AddComponent(new CTransform);
				}
				break;
			case COMPONENT_TYPE::CAMERA:
				{
					m_pTargetObject->AddComponent(new CCamera);
					m_pTargetObject->Camera()->CheckLayerMaskAll(); // 카메라가 전체 레이어를 찍도록 설정 
				}
				break;
			case COMPONENT_TYPE::COLLIDER2D:
				{
					if (nullptr == m_pTargetObject->GetRenderComponent())
					{
						m_pTargetObject->AddComponent(new CMeshRender);
						m_pTargetObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
						m_pTargetObject->MeshRender()->
						                 SetSharedMaterial(CResMgr::GetInst()->FindRes<
							                                   CMaterial>(L"material\\Std2DMtrl.mtrl"),
						                                   0);
						float fLimit = 0.3333f;
						m_pTargetObject->MeshRender()->GetSharedMaterial(0)->SetScalarParam(SCALAR_PARAM::FLOAT_0,
							&fLimit);

						m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER]->Activate();
						m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER]->SetTargetObject(m_pTargetObject);
					}

					m_pTargetObject->AddComponent(new CCollider2D);
					m_pTargetObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::BOX);
					m_pTargetObject->Collider2D()->SetOffsetPos(Vec2(0.f, 0.f));
					m_pTargetObject->Collider2D()->SetOffsetScale(Vec2(100.f, 100.f));
				}
				break;
			case COMPONENT_TYPE::COLLIDER3D:
				{
					m_pTargetObject->AddComponent(new CCollider3D);
				}
				break;
			case COMPONENT_TYPE::NAVIMAP:
				{
					m_pTargetObject->AddComponent(new CNaviMap);
				}
				break;
			case COMPONENT_TYPE::NAVIAGENT:
				{
					m_pTargetObject->AddComponent(new CNaviMap);
				}
				break;
			case COMPONENT_TYPE::ANIMATOR2D:
				{
					if (nullptr == m_pTargetObject->GetRenderComponent())
					{
						m_pTargetObject->AddComponent(new CMeshRender);
						m_pTargetObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
						m_pTargetObject->MeshRender()->
						                 SetSharedMaterial(CResMgr::GetInst()->FindRes<
							                                   CMaterial>(L"material\\Std2DMtrl.mtrl"),
						                                   0);
						float fLimit = 0.3333f;
						m_pTargetObject->MeshRender()->GetSharedMaterial(0)->SetScalarParam(SCALAR_PARAM::FLOAT_0,
							&fLimit);

						m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER]->Activate();
						m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER]->SetTargetObject(m_pTargetObject);

		

					}

					m_pTargetObject->AddComponent(new CAnimator2D);
				}
				break;
			case COMPONENT_TYPE::ANIMATOR3D:
				break;
			case COMPONENT_TYPE::LIGHT2D:
				break;
			case COMPONENT_TYPE::LIGHT3D:
				{
					if (nullptr != m_pTargetObject->GetRenderComponent())
						break;

					m_pTargetObject->AddComponent(new CLight3D);

					m_pTargetObject->Light3D()->SetLightType(LIGHT_TYPE::POINT);
					m_pTargetObject->Light3D()->SetRange(1000.f);

					m_pTargetObject->Light3D()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
					m_pTargetObject->Light3D()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));
					m_pTargetObject->Light3D()->SetAmbient(Vec3(0.f, 0.f, 0.f));
				}
				break;
			case COMPONENT_TYPE::BOUNDINGBOX:
			{
				m_pTargetObject->AddComponent(new CBoundingBox);
				CRenderComponent* pRenderCom = m_pTargetObject->GetRenderComponent();
				if(pRenderCom)
					pRenderCom->SetFrustumCulling(true);

			}
				break;
			case COMPONENT_TYPE::MESHRENDER:
				{
					if (nullptr != m_pTargetObject->GetRenderComponent())
						break;

					m_pTargetObject->AddComponent(new CMeshRender);
					m_pTargetObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
					m_pTargetObject->MeshRender()->
					                 SetSharedMaterial(CResMgr::GetInst()->FindRes<
						                                   CMaterial>(L"material\\Std3DMtrl.mtrl"),
					                                   0);

					if (m_pTargetObject->BoundingBox())
					{
						m_pTargetObject->GetRenderComponent()->SetFrustumCulling(true);
					}

				}
				break;
			case COMPONENT_TYPE::TILEMAP:
				{
					if (nullptr != m_pTargetObject->GetRenderComponent())
						break;

					m_pTargetObject->AddComponent(new CTileMap);

					Ptr<CTexture> pTileAtlas = CResMgr::GetInst()->Load<CTexture>(L"texture\\TILE_32.bmp",
						L"texture\\TILE_32.bmp");
					m_pTargetObject->TileMap()->SetAtlasTex(pTileAtlas);
					m_pTargetObject->TileMap()->SetTileSize(Vec2(30.f, 30.f));
					m_pTargetObject->TileMap()->SetTileMapCount(12, 20);

					m_pTargetObject->Transform()->SetRelativeScale(12 * 30.f, 20 * 30.f, 1.f);
				}
				break;
			case COMPONENT_TYPE::PARTICLESYSTEM:
				{
					if (nullptr != m_pTargetObject->GetRenderComponent())
						break;

					m_pTargetObject->AddComponent(new CParticleSystem);
				}
				break;
			case COMPONENT_TYPE::LANDSCAPE:
				{
					if (nullptr != m_pTargetObject->GetRenderComponent())
						break;

					m_pTargetObject->AddComponent(new CLandScape);

					m_pTargetObject->Transform()->SetRelativePos(0.f, 0.f, 0.f);
					m_pTargetObject->Transform()->SetRelativeScale(1000.f, 2000.f, 1000.f);
					m_pTargetObject->Transform()->SetRelativeRotation(0.f, 0.f, 0.f);

					m_pTargetObject->LandScape()->SetDynamicShadow(true);
					m_pTargetObject->LandScape()->SetFrustumCulling(false);
					m_pTargetObject->LandScape()->SetFaceCount(32, 32);
					m_pTargetObject->LandScape()->Create();
				}
				break;
			case COMPONENT_TYPE::DECAL:
				{
					if (nullptr != m_pTargetObject->GetRenderComponent())
						break;

					m_pTargetObject->AddComponent(new CDecal);
				}
				break;
			case COMPONENT_TYPE::SKYBOX:
				{
					if (nullptr != m_pTargetObject->GetRenderComponent())
						break;

					Ptr<CTexture> pSkyTex_03 = CResMgr::GetInst()->Load<CTexture>(L"texture\\skybox\\SkyDawn.dds",
						L"texture\\skybox\\SkyDawn.dds");
					m_pTargetObject->AddComponent(new CSkyBox);

					m_pTargetObject->SkyBox()->SetSkyboxType(SKYBOX_TYPE::CUBE);
					//m_pTargetObject->SkyBox()->GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_CUBE_0, pSkyTex_03);
					m_pTargetObject->SkyBox()->SetFrustumCulling(false);
				}
				break;
			default:
				return;
			}
			// New Component 를 UI에도 활성화 
			if (m_pTargetObject->GetComponent(eComType))
			{
				m_arrComUI[i]->Activate();
				m_arrComUI[i]->SetTargetObject(m_pTargetObject);
			}
		}
	}
}

// _param : string ( Script Type Name )
void InspectorUI::AddScript(DWORD_PTR _param)
{
	if (m_pTargetObject == nullptr)
		return;

	string  strScriptType  = (char*)_param;
	wstring wstrSCriptType = wstring(strScriptType.begin(), strScriptType.end());

	m_pTargetObject->AddComponent((CComponent*)CScriptMgr::GetScript(wstrSCriptType));
	CScript* pScript = m_pTargetObject->GetScriptByName(wstrSCriptType);
	if (pScript != nullptr)
		pScript->start();


	// UI 갱신 
	const vector<CScript*>& vecScripts = m_pTargetObject->GetScripts();
	ScriptUI*               pScriptUI  = nullptr;

	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		if (m_vecScriptUI.size() <= i)
			pScriptUI = AddScriptUI();
		else
			pScriptUI = m_vecScriptUI[i];

		// == todo ==
		// 삭제시키기 위해서 title로 이름 저장 
		wstring ScriptName = CScriptMgr::GetScriptName(vecScripts[i]);
		pScriptUI->SetTitle(ToString(ScriptName));

		pScriptUI->SetTargetObject(m_pTargetObject);
		pScriptUI->SetTargetScript(vecScripts[i]);
		pScriptUI->Activate();
	}

	// ScriptUI 가 더 많이 있을때
	if (vecScripts.size() < m_vecScriptUI.size())
	{
		// 대응하는 UI 를 제외한 나머지 ScriptUI 들을 비활성화 한다.ㄴ
		for (size_t i = vecScripts.size(); i < m_vecScriptUI.size(); ++i)
		{
			m_vecScriptUI[i]->Deactivate();
		}
	}
}

// _param : COMPONENT_TYPE
void InspectorUI::DeleteComponent(DWORD_PTR _param)
{
	COMPONENT_TYPE eComType = (COMPONENT_TYPE)_param;
	// Target Object 에서 해당 Component 를 삭제 
	m_pTargetObject->DeleteComponent(eComType);

	// UI 갱신 
	m_arrComUI[(UINT)eComType]->Deactivate();
	m_arrComUI[(UINT)eComType]->SetTargetObject(nullptr);
}

// _param : CSCript*
void InspectorUI::DeleteScript(DWORD_PTR _param)
{
	CScript* pScript = (CScript*)_param;

	// 해당 Script 삭제 
	wstring wstrScriptName = CScriptMgr::GetScriptName(pScript);
	string  strScriptName  = ToString(wstrScriptName);
	m_pTargetObject->DeleteScript(pScript->GetID());

	// UI 갱신 
	for (int i = 0; i < m_vecScriptUI.size(); ++i)
	{
		if (m_vecScriptUI[i]->GetTitle() == strScriptName)
		{
			DeleteChild(m_vecScriptUI[i]);
			m_vecScriptUI.erase(m_vecScriptUI.begin() + i);
			break;
		}
	}
}
