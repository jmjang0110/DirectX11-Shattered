#include "pch.h"
#include "GameProgressBarUI.h"

#include "CImGuiMgr.h"
#include "UITool.h"

GameProgressBarUI::GameProgressBarUI(): ComponentUI{"GameProgressBarUI", COMPONENT_TYPE::UIPROGRESSBAR} {}
GameProgressBarUI::~GameProgressBarUI() = default;

void GameProgressBarUI::render_update()
{
	ComponentUI::render_update();

	if (ImGui::Button("Open UITool"))
	{
		auto pUITool = static_cast<UITool*>(CImGuiMgr::GetInst()->FindUI("##UITool"));
		pUITool->SetTitle("UI TOOL");

		if (false == pUITool->IsActive())
		{
			pUITool->Activate();
		}
	}
}
