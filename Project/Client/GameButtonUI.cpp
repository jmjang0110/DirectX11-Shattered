#include "pch.h"
#include "GameButtonUI.h"

#include "CImGuiMgr.h"
#include "UITool.h"

GameButtonUI::GameButtonUI(): ComponentUI{"GameButtonUI", COMPONENT_TYPE::UIBUTTON} {}
GameButtonUI::~GameButtonUI() = default;

void GameButtonUI::render_update()
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
