#include "pch.h"
#include "MeshRenderUI.h"

#include "CImGuiMgr.h"
#include "ListUI.h"

#include <Engine/CResMgr.h>
#include <Engine/CMeshRender.h>
#include <Engine/CMesh.h>
#include <Engine/CMaterial.h>

MeshRenderUI::MeshRenderUI()
	:
	ComponentUI("MeshRender", COMPONENT_TYPE::MESHRENDER)
{
	SetSize(Vec2(0.f, 100.f));
}

MeshRenderUI::~MeshRenderUI() {}

void MeshRenderUI::update()
{
	ComponentUI::update();
}

void MeshRenderUI::render_update()
{
	ComponentUI::render_update();
	if (ComponentUI::IsFold()) { SetSize(Vec2(0.f, 25.f)); return; }
	else { SetSize(Vec2(0.f, 100.f)); }

	ImGui::PushItemWidth(200);

	CMeshRender*   pMeshRender = GetTargetObject()->MeshRender();
	Ptr<CMesh>     pMesh       = pMeshRender->GetMesh();
	Ptr<CMaterial> pMtrl       = pMeshRender->GetMaterial(0);

	string strMeshName, strMtrlName;
	if (nullptr != pMesh)
		strMeshName = ToString(pMesh->GetKey());
	if (nullptr != pMtrl)
		strMtrlName = ToString(pMtrl->GetKey());

	ImGui::Text("Mesh");
	ImGui::SameLine(86.f);

	ImGui::InputText("##MeshName", (char*)strMeshName.c_str(), strMeshName.capacity(), ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget())
	{
		DWORD_PTR dwData = 0;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource"))
		{
			memcpy(&dwData, payload->Data, sizeof(DWORD_PTR));
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::Button("##MeshListBtn", Vec2(15, 15)))
	{
		// ListUI 활성화한다.
		const map<wstring, CRes*>& mapRes  = CResMgr::GetInst()->GetResList(RES_TYPE::MESH);
		ListUI*                    pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->Clear();
		pListUI->SetTitle("Mesh List");

		for (const auto& pair : mapRes)
		{
			pListUI->AddList(ToString(pair.first));
		}

		pListUI->Activate();
		pListUI->SetDBCEvent(this, (DBCLKED)&MeshRenderUI::MeshSelect);
	}


	ImGui::Text("Material");
	ImGui::SameLine(86.f);
	ImGui::InputText("##MaterialName",
	                 (char*)strMtrlName.c_str(),
	                 strMtrlName.capacity(),
	                 ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();
	if (ImGui::Button("##MaterialListBtn", Vec2(15, 15)))
	{
		// ListUI 활성화한다.
		const map<wstring, CRes*>& mapRes  = CResMgr::GetInst()->GetResList(RES_TYPE::MATERIAL);
		ListUI*                    pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->Clear();
		pListUI->SetTitle("Material List");

		for (const auto& pair : mapRes)
		{
			pListUI->AddList(ToString(pair.first));
		}

		pListUI->Activate();
		pListUI->SetDBCEvent(this, (DBCLKED)&MeshRenderUI::MtrlSelect);
	}


	bool bDynamicShadow = GetTargetObject()->GetRenderComponent()->IsDynamicShadow();
	ImGui::Checkbox("Shadow", &bDynamicShadow);
	if (bDynamicShadow)
		GetTargetObject()->GetRenderComponent()->SetDynamicShadow(true);
	else
		GetTargetObject()->GetRenderComponent()->SetDynamicShadow(false);

}

void MeshRenderUI::MeshSelect(DWORD_PTR _param)
{
	string  strSelectedName = (char*)_param;
	wstring strMeshKey      = wstring(strSelectedName.begin(), strSelectedName.end());

	Ptr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(strMeshKey);
	assert(pMesh.Get());

	CMeshRender* pMeshRender = GetTargetObject()->MeshRender();
	pMeshRender->SetMesh(pMesh);
}

void MeshRenderUI::MtrlSelect(DWORD_PTR _param)
{
	string  strSelectedName = (char*)_param;
	wstring strMtrlKey      = ToWString(strSelectedName);

	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(strMtrlKey);
	assert(pMtrl.Get());

	CMeshRender* pMeshRender = GetTargetObject()->MeshRender();
	pMeshRender->SetSharedMaterial(pMtrl, 0);
}
