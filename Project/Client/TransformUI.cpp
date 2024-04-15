#include "pch.h"
#include "TransformUI.h"

#include <Engine/CTransform.h>

TransformUI::TransformUI()
	: ComponentUI("Transform", COMPONENT_TYPE::TRANSFORM)
{
	SetSize(Vec2(0.f, 90.f));
}

TransformUI::~TransformUI() {}

void TransformUI::update()
{
	ComponentUI::update();

	CGameObject* pTargetObj = GetTargetObject();
	if (nullptr == pTargetObj)
		return;

	CTransform* pTransform = pTargetObj->Transform();

	m_vRelativePos   = pTransform->GetRelativePos();
	m_vRelativeScale = pTransform->GetRelativeScale();
	m_vRelativeRot   = pTransform->GetRelativeRotation();
}

void TransformUI::render_update()
{
	ComponentUI::render_update();
	if (IsFold())
	{
		SetSize(Vec2(0.f, 25.f));
		return;
	}
	if (GetTargetObject()->GetParent() == nullptr)
		SetSize(Vec2(0.f, 105.f));
	else
		SetSize(Vec2(0.f, 130.f));

	CGameObject* pTargetObject = GetTargetObject();
	CTransform*  pTrans        = pTargetObject->Transform();
	Vec3         vPos          = pTrans->GetRelativePos();
	Vec3         vScale        = pTrans->GetRelativeScale();
	Vec3         vRot          = pTrans->GetRelativeRotation();
	vRot.ToDegree();


	ImGui::PushItemWidth(200); // Float3 위젯 간격 설정
	ImGui::Text("[ RELATIVE ]");
	ImGui::SameLine();

	ImGui::BeginGroup();
	ImGui::RadioButton("TRANSLATE", (int*)&s_guizmoType, (int)GUIZMO_TYPE::TRANSLATE);
	ImGui::SameLine();
	ImGui::RadioButton("ROTATE", (int*)&s_guizmoType, (int)GUIZMO_TYPE::ROTATE);
	ImGui::SameLine();
	ImGui::RadioButton("SCALE", (int*)&s_guizmoType, (int)GUIZMO_TYPE::SCALE);
	ImGui::EndGroup();

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
	pTrans->SetRelativePos(vPos);


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
	pTrans->SetRelativeScale(vScale);


	ImGui::Text("Rotation", 100.f);
	ImGui::SameLine();

	ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "X");
	ImGui::SameLine(0.f);
	ImGui::PushItemWidth(60);
	ImGui::DragFloat("##Rotation X_drag", &vRot.x);
	ImGui::SameLine();
	ImGui::PopItemWidth();

	ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Y");
	ImGui::SameLine(0.f);
	ImGui::PushItemWidth(60);
	ImGui::DragFloat("##Rotation Y_drag", &vRot.y);
	ImGui::SameLine();
	ImGui::PopItemWidth();

	ImGui::TextColored(ImVec4(0.1f, 0.4f, 1.f, 1.f), "Z");
	ImGui::SameLine(0.f);
	ImGui::PushItemWidth(60);
	ImGui::DragFloat("##Rotation Z_drag", &vRot.z);
	ImGui::PopItemWidth();
	vRot.ToRadian();
	pTrans->SetRelativeRotation(vRot);

	if (nullptr != pTargetObject->GetParent())
	{
		m_bIgnorantParentScale = pTrans->GetIgnorantParentScale();
		ImGui::Checkbox("IgnoreParentScale", &m_bIgnorantParentScale);
		{
			pTrans->SetIgnoreParentScale(m_bIgnorantParentScale);
		}
	}
}
