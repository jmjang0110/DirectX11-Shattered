#include "pch.h"
#include "GameTextUI.h"

#include "CImGuiMgr.h"
#include "UITool.h"

GameTextUI::GameTextUI(): ComponentUI{"GameTextUI", COMPONENT_TYPE::UITEXT} {}
GameTextUI::~GameTextUI() = default;

void GameTextUI::render_update()
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
