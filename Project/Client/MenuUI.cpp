#include "pch.h"
#include "MenuUI.h"

#include <Engine/CCore.h>
#include <Engine/CPathMgr.h>

#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Script/CScriptMgr.h>

#include <Script/CSceneSaveLoad.h>
#include "CImGuiMgr.h"
#include "SceneOutliner.h"
#include "ParticleTool.h"
#include "FogTool.h"
#include "UITool.h"

MenuUI::MenuUI()
	: UI("Menu")
	, m_bPackaging(false)
	, m_bSceneSave(false)
	, m_bSceneLoad(false)
	, m_bScenePlayPause(false)
	, m_bSceneStop(false)
	, m_bParticleTool(false)
	, m_bFogTool(false)
	, m_bUITool{false} {}

MenuUI::~MenuUI() = default;

void MenuUI::update()
{
	Task();
}

void MenuUI::render()
{
	if (ImGui::BeginMainMenuBar())
	{
		render_update();

		ImGui::EndMainMenuBar();
	}
}

void MenuUI::render_update()
{
	if (ImGui::BeginMenu(u8"파일"))
	{
		ImGui::MenuItem("Packaging", nullptr, &m_bPackaging);
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Scene"))
	{
		ImGui::MenuItem("Scene Save", nullptr, &m_bSceneSave);
		ImGui::MenuItem("Scene Load", nullptr, &m_bSceneLoad);

		const CScene*     pCurScene = CSceneMgr::GetInst()->GetCurScene();
		const SCENE_STATE eState    = pCurScene->GetSceneState();

		m_strPlayPause = SCENE_STATE::PLAY == eState ? "Pause" : "Play";
		ImGui::MenuItem(m_strPlayPause.c_str(), nullptr, &m_bScenePlayPause);

		if (SCENE_STATE::STOP == eState)
		{
			ImGui::MenuItem("Stop", nullptr, &m_bSceneStop, false);
		}
		else
		{
			ImGui::MenuItem("Stop", nullptr, &m_bSceneStop, true);
		}
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Component"))
	{
		if (ImGui::BeginMenu("Add Component"))
		{
			ImGui::MenuItem("MeshRender", nullptr);
			ImGui::MenuItem("Camera", nullptr);
			ImGui::MenuItem("Collider2D", nullptr);
			ImGui::MenuItem("Collider3D", nullptr);
			ImGui::MenuItem("Animator2D", nullptr);
			ImGui::MenuItem("Animator3D", nullptr);

			if (ImGui::BeginMenu("Add Script"))
			{
				vector<wstring> vecScriptName{};
				CScriptMgr::GetScriptInfo(vecScriptName);
				for (auto& scriptName : vecScriptName)
				{
					ImGui::MenuItem(ToString(scriptName).c_str(), nullptr);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu(u8"제작 툴"))
	{
		ImGui::MenuItem(u8"파티클", nullptr, &m_bParticleTool);
		ImGui::MenuItem("Fog Tool", nullptr, &m_bFogTool);
		ImGui::MenuItem(u8"UI", nullptr, &m_bUITool);

		ImGui::EndMenu();
	}
}


void MenuUI::Task()
{
	if (m_bSceneSave)
	{
		wchar_t szName[256] = {};

		OPENFILENAME ofn = {};

		ofn.lStructSize    = sizeof(OPENFILENAME);
		ofn.hwndOwner      = CCore::GetInst()->GetMainHwnd();
		ofn.lpstrFile      = szName;
		ofn.nMaxFile       = sizeof(szName);
		ofn.lpstrFilter    = L"ALL\0*.*\0Scene\0*.scene\0";
		ofn.nFilterIndex   = 0;
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle  = 0;

		wstring strTileFolder = CPathMgr::GetInst()->GetContentPath();
		strTileFolder += L"scene";

		ofn.lpstrInitialDir = strTileFolder.c_str();
		ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		// Modal
		if (GetSaveFileName(&ofn))
		{
			CSceneSaveLoad::SaveScene(CSceneMgr::GetInst()->GetCurScene(), szName);
		}

		m_bSceneSave = false;
	}

	else if (m_bSceneLoad)
	{
		wchar_t szName[256] = {};

		OPENFILENAME ofn = {};

		ofn.lStructSize    = sizeof(OPENFILENAME);
		ofn.hwndOwner      = CCore::GetInst()->GetMainHwnd();
		ofn.lpstrFile      = szName;
		ofn.nMaxFile       = sizeof(szName);
		ofn.lpstrFilter    = L"ALL\0*.*\0Tile\0*.tile\0";
		ofn.nFilterIndex   = 0;
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle  = 0;

		wstring strTileFolder = CPathMgr::GetInst()->GetContentPath();
		strTileFolder += L"tile";

		ofn.lpstrInitialDir = strTileFolder.c_str();
		ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		// Modal
		if (GetOpenFileName(&ofn))
		{
			CScene* pLoadScene = CSceneSaveLoad::LoadScene(szName);
			CSceneMgr::GetInst()->ChangeScene(pLoadScene);
		}

		// SceneOutliner 갱신
		auto pUI = static_cast<SceneOutliner*>(CImGuiMgr::GetInst()->FindUI("SceneOutliner"));
		pUI->Reset();

		m_bSceneLoad = false;
	}

	if (m_bScenePlayPause)
	{
		CScene*     pCurScene = CSceneMgr::GetInst()->GetCurScene();
		SCENE_STATE eState    = pCurScene->GetSceneState();

		if (SCENE_STATE::STOP == eState)
		{
			pCurScene->SetSceneState(SCENE_STATE::PLAY);
		}
		else if (SCENE_STATE::PLAY == eState)
		{
			pCurScene->SetSceneState(SCENE_STATE::PAUSE);
		}

		m_bScenePlayPause = false;
	}

	if (m_bSceneStop)
	{
		CScene*     pCurScene = CSceneMgr::GetInst()->GetCurScene();
		SCENE_STATE eState    = pCurScene->GetSceneState();

		if (SCENE_STATE::STOP != eState)
		{
			pCurScene->SetSceneState(SCENE_STATE::STOP);
			CSceneFile* pSceneFile = pCurScene->GetSceneFile().Get();

			wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + pSceneFile->GetRelativePath();
			CScene* pNewScene   = CSceneSaveLoad::LoadScene(strFilePath);
			CSceneMgr::GetInst()->ChangeScene(pNewScene);
		}

		// SceneOutliner 갱신
		static_cast<SceneOutliner*>(CImGuiMgr::GetInst()->FindUI("SceneOutliner"))->Reset();

		m_bSceneStop = false;
	}

	if (m_bParticleTool)
	{
		auto pParticleToolUI = static_cast<ParticleTool*>(CImGuiMgr::GetInst()->FindUI("##ParticleToolUI"));
		pParticleToolUI->SetTitle("Particle Tool");
		if (nullptr == pParticleToolUI)
			return;
		pParticleToolUI->Activate();
		m_bParticleTool = false;
	}

	if (m_bFogTool)
	{
		auto pFogToolUI = static_cast<FogTool*>(CImGuiMgr::GetInst()->FindUI("##FogTool"));
		pFogToolUI->SetTitle("Fog Tool");
		if (nullptr == pFogToolUI)
			return;
		pFogToolUI->Activate();
		m_bFogTool = false;
	}

	if (m_bUITool)
	{
		auto pUITool = static_cast<UITool*>(CImGuiMgr::GetInst()->FindUI("##UITool"));
		pUITool->SetTitle("UI TOOL");

		if (false == pUITool->IsActive())
		{
			pUITool->Activate();
		}
		m_bUITool = false;
	}
}
