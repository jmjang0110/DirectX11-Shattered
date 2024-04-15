#include "pch.h"
#include "GameImageUI.h"

#include "CImGuiMgr.h"
#include "UITool.h"

GameImageUI::GameImageUI(): ComponentUI{"GameImageUI", COMPONENT_TYPE::UIIMAGE} {}
GameImageUI::~GameImageUI() = default;

void GameImageUI::render_update()
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
