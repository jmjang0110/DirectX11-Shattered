#include "pch.h"
#include "BoundingBoxUI.h"
#include <Engine/CBoundingBox.h>

BoundingBoxUI::BoundingBoxUI()
	: ComponentUI{ "BoundingBoxUI", COMPONENT_TYPE::BOUNDINGBOX }
{
}

BoundingBoxUI::~BoundingBoxUI()
{
}

void BoundingBoxUI::render_update()
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
	CBoundingBox* pBoundingBox = pTargetObject->BoundingBox();
	Vec3				 vPos  = pBoundingBox->GetOffsetPos();
	Vec3				vScale = pBoundingBox->GetOffsetScale();

	ImGui::PushItemWidth(200); // Float3 위젯 간격 설정
	ImGui::Text("[ RELATIVE ]");
	ImGui::Text("Position", 100.f);
	ImGui::SameLine();

	ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "X");
	ImGui::SameLine(0.f);
	ImGui::PushItemWidth(60);
	ImGui::DragFloat("##LeftTop X_drag", &vPos.x);
	ImGui::SameLine();
	ImGui::PopItemWidth();

	ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Y");
	ImGui::SameLine(0.f);
	ImGui::PushItemWidth(60);
	ImGui::DragFloat("##LeftTop Y_drag", &vPos.y);
	ImGui::SameLine();
	ImGui::PopItemWidth();

	ImGui::TextColored(ImVec4(0.1f, 0.4f, 1.f, 1.f), "Z");
	ImGui::SameLine(0.f);
	ImGui::PushItemWidth(60);
	ImGui::DragFloat("##LeftTop Z_drag", &vPos.z);
	ImGui::PopItemWidth();
	pBoundingBox->SetOffsetPos(vPos);

	ImGui::Text("Scale     ", 100.f);
	ImGui::SameLine();

	ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "X");
	ImGui::SameLine(0.f);
	ImGui::PushItemWidth(60);
	ImGui::DragFloat("##Scale X_drag", &vScale.x);
	ImGui::SameLine();
	ImGui::PopItemWidth();

	ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Y");
	ImGui::SameLine(0.f);
	ImGui::PushItemWidth(60);
	ImGui::DragFloat("##Scale Y_drag", &vScale.y);
	ImGui::SameLine();
	ImGui::PopItemWidth();

	ImGui::TextColored(ImVec4(0.1f, 0.4f, 1.f, 1.f), "Z");
	ImGui::SameLine(0.f);
	ImGui::PushItemWidth(60);
	ImGui::DragFloat("##Scale Z_drag", &vScale.z);
	ImGui::PopItemWidth();

	pBoundingBox->SetOffsetScale(vScale);
}
