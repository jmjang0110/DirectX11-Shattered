#include "pch.h"
#include "NaviAgentUI.h"

#include <Engine/CNaviAgent.h>
#include <Engine/CNaviMap.h>

NaviAgentUI::NaviAgentUI()
	: ComponentUI{"NaviAgent", COMPONENT_TYPE::NAVIAGENT}
{
}

NaviAgentUI::~NaviAgentUI()
{
}

void NaviAgentUI::render_update()
{
	ComponentUI::render_update();
	if (ComponentUI::IsFold())
	{
		SetSize(Vec2(0.f, 25.f));
		return;
	}
	else
	{
		if (GetTargetObject()->GetParent() == nullptr)
			SetSize(Vec2(0.f, 105.f));
		else
			SetSize(Vec2(0.f, 130.f));
	}

	CGameObject* pTargetObject = GetTargetObject();
	CNaviAgent*  pAgent        = pTargetObject->NaviAgent();

	// Target NaviMap GameObject Name
	string strMapName = "No Navi Map";
	if (nullptr != pAgent->GetNaviMap())
		strMapName = ToString(pAgent->GetNaviMap()->GetOwner()->GetName());

	ImGui::Text("NaviMap");
	ImGui::SameLine(86.f);
	ImGui::InputText("##NaviMapName",
	                 (char*)strMapName.c_str(),
	                 strMapName.capacity(),
	                 ImGuiInputTextFlags_ReadOnly);

	// Agent Pos
	float vPos[3] = {pAgent->GetOffsetPos().x, pAgent->GetOffsetPos().y, pAgent->GetOffsetPos().z};
	ImGui::Text("Agent Pos");
	if (ImGui::DragFloat3("##AgentPos", vPos))
	{
		pAgent->SetOffsetPos(Vec3(vPos[0], vPos[1], vPos[2]));
	}

	// Agent Size
	float vSize[3] = {pAgent->GetOffsetSize().x, pAgent->GetOffsetSize().y, pAgent->GetOffsetSize().z};
	ImGui::Text("Agent Size");
	if (ImGui::DragFloat3("##AgentSize", vSize))
	{
		pAgent->SetOffsetSize(Vec3(vSize[0], vSize[1], vSize[2]));
	}

	// Cur Cell Index
	int index = pAgent->GetCurCellIndex();
	ImGui::Text("Cur Index");
	ImGui::InputInt("##CurIndex", &index, 1, 100, ImGuiInputTextFlags_ReadOnly);

	// Gravity
	//bool useGravity = pAgent->GetGravity();
	//ImGui::Text("Use Gravity");
	//if (ImGui::Checkbox("##Gravity", &useGravity))
	//{
	//	pAgent->SetGravity(useGravity);
	//}
}
