#include "pch.h"
#include "DecalUI.h"

#include <Engine/CDecal.h>
#include <Engine/CTexture.h>

#include "ListUI.h"
#include "CImGuiMgr.h"

DecalUI::DecalUI()
	: ComponentUI{"Decal", COMPONENT_TYPE::DECAL}
{
	SetSize(Vec2{0, 100});
}

DecalUI::~DecalUI() = default;

void DecalUI::render_update()
{
	ComponentUI::render_update();
	if (IsFold())
	{
		SetSize(Vec2(0.f, 25.f));
		return;
	}
	SetSize(Vec2(0.f, 100.f));

	// Target Decal 설정
	m_pTargetDecal = GetTargetObject()->Decal();

	// Decal Types
	const string strCurTypeName = m_pTargetDecal->GetDecalTypeName(m_pTargetDecal->GetDecalType());

	ImGui::Text("Decal Type");
	ImGui::SameLine(100.f);

	if (ImGui::BeginCombo("##iDecalType", strCurTypeName.c_str()))
	{
		for (int i = 0; i < static_cast<int>(DECAL_TYPE::END); i++)
		{
			string strTypeName = m_pTargetDecal->GetDecalTypeName(static_cast<DECAL_TYPE>(i));
			if (ImGui::Selectable(strTypeName.c_str()))
			{
				m_pTargetDecal->SetDecalType(static_cast<DECAL_TYPE>(i));
			}
		}

		ImGui::EndCombo();
	}

	// Texture 설정
	string strTexture;

	if (nullptr != m_pTargetDecal->GetDecalTexture())
		strTexture = ToString(m_pTargetDecal->GetDecalTexture()->GetKey());
	else
		strTexture = "Please Select Texture >>";

	ImGui::Text("Texture");
	ImGui::SameLine(100.f);

	ImGui::InputText("##CurDecalTex",
	                 (char*)strTexture.c_str(),
	                 256,
	                 ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();
	if (ImGui::Button("##SelectDecalTex", Vec2(15, 15)))
	{
		// ListUI 활성화한다.
		const auto& mapRes  = CResMgr::GetInst()->GetResList(RES_TYPE::TEXTURE);
		auto        pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
		pListUI->Clear();
		pListUI->SetTitle("TEXTURE_LIST");

		for (const auto& pairData : mapRes)
		{
			pListUI->AddList(ToString(pairData.first));
		}

		pListUI->Activate();
		pListUI->SetDBCEvent(this, (DBCLKED)&DecalUI::TextureSelect);
	}

	// UseDeferredLighting 설정
	bool bUseDeferredLighting = m_pTargetDecal->IsUsingDeferredLighting();
	ImGui::Text("Deferred Lighting");
	ImGui::SameLine(100.f);
	if (ImGui::Checkbox("##UseDefLighting", &bUseDeferredLighting))
	{
		m_pTargetDecal->SetDeferredLighting(bUseDeferredLighting);
	}

	// color
	Vec4  vColor   = m_pTargetDecal->GetColor();
	float color[4] = {vColor.x, vColor.y, vColor.z, vColor.w};
	ImGui::Text("Color");
	ImGui::SameLine(100.f);
	if (ImGui::ColorPicker4("##DecalColor", color, ImGuiColorEditFlags_AlphaBar))
	{
		m_pTargetDecal->SetColor(Vec4(color[0], color[1], color[2], color[3]));
	}
}

void DecalUI::TextureSelect(void* _pTextureName)
{
	const std::wstring  key      = ToWString(static_cast<char*>(_pTextureName));
	const Ptr<CTexture> pTexture = CResMgr::GetInst()->FindRes<CTexture>(key);

	// 변경점이 있을 때만 세팅
	if (m_pTargetDecal->GetDecalTexture() != pTexture)
	{
		m_pTargetDecal->SetDecalTexture(pTexture);
	}
}
