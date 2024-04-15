#include "pch.h"
#include "Collider3DUI.h"

#include <Engine/CCollider3D.h>
#include <Script/SocketColliderScript.h>


Collider3DUI::Collider3DUI()
	: ComponentUI{"Collider3D", COMPONENT_TYPE::COLLIDER3D}
{
}

Collider3DUI::~Collider3DUI()
{
}

void Collider3DUI::render_update()
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

	CGameObject*    pTargetObject = GetTargetObject();
	CCollider3D*    pCollider3D   = pTargetObject->Collider3D();
	Vec3            vPos          = pCollider3D->GetOffsetPos();
	Vec3            vScale        = pCollider3D->GetOffsetScale();
	COLLIDER3D_TYPE type          = pCollider3D->GetCollider3DType();

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
	pCollider3D->SetOffsetPos(vPos);

	ImGui::Text("Scale     ", 100.f);
	ImGui::SameLine();

	if (COLLIDER3D_TYPE::CUBE == type)
	{
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
	}
	else
	{
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Radius");
		ImGui::SameLine(0.f);
		ImGui::PushItemWidth(60);
		ImGui::DragFloat("##Scale_SPHERE_drag", &vScale.z);
		ImGui::PopItemWidth();
	}
	pCollider3D->SetOffsetScale(vScale);

	static const char* s_arrColliderTypeName[] = {"CUBE", "SPHERE"};
	if (ImGui::BeginCombo("##Collider3D_COMBO", s_arrColliderTypeName[(int)type]))
	{
		for (size_t i = 0; i < std::size(s_arrColliderTypeName); ++i)
		{
			if (ImGui::Selectable(s_arrColliderTypeName[i]))
			{
				pCollider3D->SetCollider3DType(static_cast<COLLIDER3D_TYPE>(i));
			}
		}
		ImGui::EndCombo();
	}


	
	SocketColliderScript* pSocketScript = (SocketColliderScript*)GetTargetObject()->GetScriptByName(L"SocketColliderScript");
	if (pSocketScript)
	{
		int SocketIdx = pSocketScript->GetSocketIdx();

		ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "SocketIdx");
		ImGui::SameLine(0.f);
		ImGui::PushItemWidth(60);
		ImGui::DragInt("##SocketIdx", &SocketIdx);
		ImGui::SameLine();
		ImGui::PopItemWidth();
		ClampData(SocketIdx, 0, (int)pSocketScript->GetvecSocketMaxSize() - 1);

		pSocketScript->SetSocketIdx(SocketIdx);
		ImGui::Text("Offset     ", 100.f);
		ImGui::SameLine();
		Vec3 vOffsetPos = pSocketScript->GetSocketOffsetPos();
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "X");
		ImGui::PushItemWidth(60);
		ImGui::DragFloat("##OffsetPos_x", &vOffsetPos.x);
		ImGui::SameLine();
		ImGui::PopItemWidth();

		ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Y");
		ImGui::SameLine(0.f);
		ImGui::PushItemWidth(60);
		ImGui::DragFloat("##OffsetPos_y", &vOffsetPos.y);
		ImGui::SameLine();
		ImGui::PopItemWidth();

		ImGui::TextColored(ImVec4(0.1f, 0.4f, 1.f, 1.f), "Z");
		ImGui::SameLine(0.f);
		ImGui::PushItemWidth(60);
		ImGui::DragFloat("##OffsetPos_z", &vOffsetPos.z);
		ImGui::PopItemWidth();

		pSocketScript->SetSocketOffsetPos(vOffsetPos);

	}

}
