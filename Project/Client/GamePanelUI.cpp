#include "pch.h"
#include "GamePanelUI.h"

#include "CImGuiMgr.h"
#include "UITool.h"

GamePanelUI::GamePanelUI(): ComponentUI{"GamePanelUI", COMPONENT_TYPE::UIPANEL} {}
GamePanelUI::~GamePanelUI() = default;

void GamePanelUI::render_update()
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
