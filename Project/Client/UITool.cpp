#include "pch.h"
#include "UITool.h"

#include <Engine/CCore.h>
#include <Engine/CGameObject.h>
#include <Engine/CTransform.h>
#include <Engine/CMeshRender.h>
#include <Engine/CSceneMgr.h>
#include <Engine/CFontMgr.h>
#include <Engine/CUIImage.h>
#include <Engine/CUIText.h>
#include <Engine/CUIButton.h>
#include <Engine/CUIProgressBar.h>
#include <Engine/CUIPanel.h>
#include <Engine/CCamera.h>
#include <Engine/CRenderMgr.h>
#include <Engine/CMRT.h>

#include "CImGuiMgr.h"
#include "ListUI.h"
#include "SceneOutliner.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui_internal.h"

namespace UI_TOOL
{
	constexpr int            DEFAULT_ITEM_SPACING = 200;
	constexpr const wchar_t* UI_LAYER             = L"UI_INTERACTIVE";
	constexpr const wchar_t* BACK_LAYER           = L"BG";
	constexpr const wchar_t* BACK_OBJ_LAYER       = L"BG_OBJ";
}

UITool::UITool()
	: UI{"##UITool"} {}

UITool::~UITool() = default;

void UITool::render_update()
{
	DrawImage();
	DrawInfo();

	if (m_bOpenWarningPopUp)
	{
		ImGui::OpenPopup("UI_WARNING");
		m_bOpenWarningPopUp = false;
	}
	DrawWarning();
}

void UITool::DrawImage()
{
	static bool op{};
	if (m_bOpenFramePopUp)
	{
		ImGui::OpenPopup(u8"UI_FRAME_POPUP");
		m_bOpenFramePopUp = false;
		op                = true;
	}

	if (ImGui::BeginPopupModal(u8"UI_FRAME_POPUP", &op))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 0});
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});
		const ImVec2 windowSize = ImGui::GetWindowSize();
		ImGui::BeginChild(u8"UI_FRAME_TEXTURE",
		                  windowSize,
		                  true,
		                  ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
		if (nullptr != m_pFrameTexture)
		{
			ImGui::Image(m_pFrameTexture->GetSRV().Get(), windowSize);
			CropImageByDrag();
		}
		ImGui::EndChild();
		ImGui::PopStyleVar(3);

		ImGui::EndPopup();
	}
}

void UITool::CropImageByDrag()
{
	const ImVec2 windowPos  = ImGui::GetWindowPos();
	const ImVec2 mousePos   = ImGui::GetMousePos();
	const ImVec2 currentPos = mousePos - windowPos;
	const ImVec2 windowSize = ImGui::GetWindowSize();
	ImDrawList*  pDrawList  = ImGui::GetWindowDrawList();

	//clicked first
	if (ImGui::IsItemHovered()
	    && false == m_isDrawing
	    && ImGui::IsMouseClicked(ImGuiMouseButton_Left)
	    && false == ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
	{
		m_isDrawing    = true;
		m_bHasDragArea = false;
		m_startPos     = mousePos - windowPos;
		ImGui::OpenPopup("OVERLAY");
	}
	//still drawing
	if (m_isDrawing)
	{
		m_lastPos = currentPos;
		CalculatePosAndSize({windowSize.x, windowSize.y});
		ImGui::SetNextWindowPos(mousePos);
		if (ImGui::BeginPopup("OVERLAY"))
		{
			ImGui::Text("LeftTop :%.f,%.f,", m_leftTop.x, m_leftTop.y);
			ImGui::Text("Size : %.1f,%.1f", m_size.x, m_size.y);

			ImGui::EndPopup();
		}

		//mouse up
		if (false == ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			m_isDrawing    = false;
			m_bHasDragArea = true;
		}
		//draw rect
		pDrawList->AddRect(m_startPos + windowPos, currentPos + windowPos, IM_COL32(0, 255, 0, 255));
	}

	static ImVec2 offsetStartPos{};
	static ImVec2 offset{};
	static ImVec2 beforeStart{};
	static ImVec2 beforeLast{};
	const bool    isInside = (m_startPos.x <= currentPos.x && currentPos.x <= m_lastPos.x
	                          && m_startPos.y <= currentPos.y && currentPos.y <= m_lastPos.y);
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)
	    && ImGui::IsKeyDown(ImGuiKey_LeftCtrl)
	    && isInside)
	{
		m_isDragging   = true;
		offsetStartPos = currentPos;
		offset         = {};
		beforeStart    = m_startPos;
		beforeLast     = m_lastPos;

		ImGui::OpenPopup("OVERLAY");
	}

	if (m_isDragging)
	{
		offset     = currentPos - offsetStartPos;
		m_startPos = beforeStart + offset;
		m_lastPos  = beforeLast + offset;

		CalculatePosAndSize({windowSize.x, windowSize.y});
		ImGui::SetNextWindowPos(mousePos);
		if (ImGui::BeginPopup("OVERLAY"))
		{
			ImGui::Text("LeftTop :%.f,%.f,", m_leftTop.x, m_leftTop.y);
			ImGui::Text("Size : %.1f,%.1f", m_size.x, m_size.y);

			ImGui::EndPopup();
		}

		if (false == ImGui::IsMouseDown(ImGuiMouseButton_Left) || false == ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
		{
			m_isDragging = false;
		}
	}

	if (m_bHasDragArea)
	{
		pDrawList->AddRect(m_startPos + windowPos, m_lastPos + windowPos, IM_COL32(0, 255, 0, 255));
	}
}

void UITool::DrawInfo()
{
	ImGui::BeginChild("UI_INFO_SECTION");

	ImGui::BeginGroup();
	if (ImGui::Button(u8"패널 생성")) { AddUI(UI_TYPE::PANEL); }
	ImGui::SameLine();
	if (ImGui::Button(u8"이미지 생성")) { AddUI(UI_TYPE::IMAGE); }
	ImGui::SameLine();
	if (ImGui::Button(u8"텍스트 생성")) { AddUI(UI_TYPE::TEXT); }
	if (ImGui::Button(u8"버튼 기능 추가")) { AddExtraFeature(UI_EXTRA_TYPE::BUTTON); }
	ImGui::SameLine();
	if (ImGui::Button(u8"프로그레스 바 기능 추가")) { AddExtraFeature(UI_EXTRA_TYPE::PROGRESS_BAR); }
	if (ImGui::Button(u8"불러오기")) { LoadUIPrefab(); }

	const SceneOutliner* pOutLiner = static_cast<SceneOutliner*>(CImGuiMgr::GetInst()->FindUI("SceneOutliner"));
	m_pSelected                    = pOutLiner->GetSelectedGameObject();
	if (m_pSelected && nullptr == m_pSelected->GetUIBaseComponenent())
	{
		m_pSelected = nullptr;
	}
	if (m_pSelected)
	{
		ImGui::SameLine();
		if (ImGui::Button(u8"프리팹으로 저장")) { SaveAsPrefab(); }

		if (m_pSelected->UIImage() || m_pSelected->UIPanel())
		{
			ImGui::PushID(0);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(150.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
			ImGui::Button(u8"[드래그 정보]");
			ImGui::PopStyleColor(3);
			ImGui::PopID();

			const Ptr<CTexture> pTexture = m_pSelected->MeshRender()->GetMaterial(0)->GetTexParam(TEX_PARAM::TEX_0);
			if (nullptr != pTexture)
			{
				ImGui::SameLine();
				if (ImGui::Button(u8"프레임 선택"))
				{
					m_pFrameTexture   = pTexture;
					m_bOpenFramePopUp = true;
				}
			}

			ImGui::Text("LEFT TOP");
			ImGui::InputFloat2("##DRAG_LEFTOP", &m_leftTop.x);
			ImGui::Text("SIZE");
			ImGui::InputFloat2("##DRAG_SIZE", &m_size.x);
		}
	}
	ImGui::EndGroup(); //BUTTON GROUP

	if (m_pSelected)
	{
		if (CUIBase* pUIBaseComponent = m_pSelected->GetUIBaseComponenent())
		{
			switch (pUIBaseComponent->GetType())
			{
			case COMPONENT_TYPE::UITEXT:
				ShowText(static_cast<CUIText*>(pUIBaseComponent));
				break;
			case COMPONENT_TYPE::UIIMAGE:
				ShowImage(static_cast<CUIImage*>(pUIBaseComponent));
				break;
			case COMPONENT_TYPE::UIPANEL:
				ShowPanel(static_cast<CUIPanel*>(pUIBaseComponent));
				break;
			}
		}

		CComponent* pProgressBar = m_pSelected->GetComponent(COMPONENT_TYPE::UIPROGRESSBAR);
		if (nullptr != pProgressBar)
		{
			ShowProgressBar(static_cast<CUIProgressBar*>(pProgressBar));
		}

		CComponent* pButton = m_pSelected->GetComponent(COMPONENT_TYPE::UIBUTTON);
		if (nullptr != pButton)
		{
			ShowButton(static_cast<CUIButton*>(pButton));
		}
	}

	ImGui::EndChild();//UI_INFO_SECTION
}

void UITool::DrawWarning()
{
	if (m_bShowWarning)
	{
		if (ImGui::BeginPopup("UI_WARNING"))
		{
			if (m_bWrongPrefab)
			{
				ImGui::Text(u8"선택한 프리팹은 UI 프리팹이 아닙니다");
				ImGui::Text(u8"다시 선택하세요");
			}
			else if (nullptr == m_pSelected)
			{
				ImGui::Text(u8"선택된 UI 객체가 없습니다");
				ImGui::Text(u8"객체를 추가하고 Button/Progress bar를 축하세요");
			}
			else if (m_bAlreadyHas)
			{
				ImGui::Text(u8"이미 Button 또는 Progress bar가 존재합니다");
			}
			else if (m_bNoImageScript)
			{
				ImGui::Text(u8"Button/ProgressBar는 UIImage/UIPanel/UIText를 가진 객체에만 가능합니다");
			}
			else
			{
				ImGui::Text(u8"선택된 UI는 Panel Component가 존재하지 않습니다.");
				ImGui::Text(u8"Component 종류를 변경하세요");
			}
			if (ImGui::Button(u8"닫기"))
			{
				m_bShowWarning   = false;
				m_bWrongPrefab   = false;
				m_bAlreadyHas    = false;
				m_bNoImageScript = false;

				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}

void UITool::TextureSelected(DWORD_PTR _pTextureName) const
{
	const auto          pTextureName = reinterpret_cast<const char*>(_pTextureName);
	const Ptr<CTexture> pTexture     = CResMgr::GetInst()->FindRes<CTexture>(ToWString(pTextureName));
	m_pSelected->MeshRender()->GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, pTexture);
}

void UITool::FontSelected(DWORD_PTR _pFontName) const
{
	const auto pFontName = reinterpret_cast<const char*>(_pFontName);
	m_pSelected->UIText()->SetFont(ToWString(pFontName));
}

void UITool::AddDelegate(DWORD_PTR _pGameObject)
{
	const auto pGameObject = reinterpret_cast<CGameObject*>(_pGameObject);

	if (nullptr == m_pSelected)
	{
		pGameObject->Transform()->SetRelativePos({0.f, 0.f, 500.f});
		CSceneMgr::GetInst()->SpawnObject(pGameObject, UI_TOOL::UI_LAYER);
	}
	else
	{
		const auto pParent = m_pSelected;
		if (nullptr == pParent->UIPanel())
		{
			m_bShowWarning      = true;
			m_bOpenWarningPopUp = true;
			pGameObject->Destroy();

			return;
		}
		CSceneMgr::GetInst()->SpawnObject(pGameObject, m_pSelected->GetLayerIndex());
		CSceneMgr::GetInst()->AddChild(m_pSelected, pGameObject);
	}

	SceneOutliner* pOutLiner = static_cast<SceneOutliner*>(CImGuiMgr::GetInst()->FindUI("SceneOutliner"));
	pOutLiner->ShowHierarchyAll(pGameObject, true);
}

void UITool::AddUI(UI_TYPE type)
{
	const std::wstring generatedName = L"UI_" + std::to_wstring(s_geneatedID++);

	auto pNewGameObject = new CGameObject{};
	pNewGameObject->SetName(generatedName);
	pNewGameObject->AddComponent(new CTransform{});
	pNewGameObject->Transform()->SetIgnoreParentScale(true);
	pNewGameObject->AddComponent(new CMeshRender{});
	pNewGameObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pNewGameObject->MeshRender()->
	                SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\UIMtrl.mtrl"), 0);

	switch (type)
	{
	case UI_TYPE::PANEL:
		pNewGameObject->AddComponent(new CUIPanel{});
		break;
	case UI_TYPE::IMAGE:
		pNewGameObject->AddComponent(new CUIImage{});
		break;
	case UI_TYPE::TEXT:
		pNewGameObject->AddComponent(new CUIText{});
		break;
	default: assert(nullptr && "INVALID UI SCRIPT TYPE");
	}

	tUIDelegate uiDelegate{};
	uiDelegate.pInst   = this;
	uiDelegate.pFunc   = static_cast<PARAM_1>(&UITool::AddDelegate);
	uiDelegate.dwParam = (DWORD_PTR)pNewGameObject;
	CImGuiMgr::GetInst()->AddDelegate(uiDelegate);
}

void UITool::AddExtraFeature(UI_EXTRA_TYPE type)
{
	if (nullptr == m_pSelected)
	{
		m_bShowWarning      = true;
		m_bOpenWarningPopUp = true;
		return;
	}

	if (nullptr == m_pSelected->GetUIBaseComponenent())
	{
		m_bShowWarning      = true;
		m_bOpenWarningPopUp = true;
		m_bNoImageScript    = true;

		return;
	}
	switch (type)
	{
	case UI_EXTRA_TYPE::BUTTON:
		{
			if (m_pSelected->UIText())
			{
				m_bShowWarning      = true;
				m_bAlreadyHas       = true;
				m_bOpenWarningPopUp = true;
			}
			else
			{
				m_pSelected->AddComponent(new CUIButton{});
			}
			break;
		}
	case UI_EXTRA_TYPE::PROGRESS_BAR:
		{
			if (m_pSelected->UIProgressBar())
			{
				m_bShowWarning      = true;
				m_bAlreadyHas       = true;
				m_bOpenWarningPopUp = true;
			}
			else
			{
				m_pSelected->AddComponent(new CUIProgressBar{});
			}
			break;
		}
	}
}

void UITool::CalculatePosAndSize(const Vec2& windowSize)
{
	//좌상단->우하단
	if (m_lastPos.x - m_startPos.x > 0 && m_lastPos.y - m_startPos.y > 0)
	{
		m_leftTop = m_startPos;
		m_size    = m_lastPos - m_startPos;
	}
	//우하단->좌상단
	else if (m_lastPos.x - m_startPos.x < 0 && m_lastPos.y - m_startPos.y < 0)
	{
		m_leftTop = m_lastPos;
		m_size    = m_startPos - m_lastPos;
	}
	//좌하단->우상단
	else if (m_lastPos.x - m_startPos.x > 0 && m_lastPos.y - m_startPos.y < 0)
	{
		m_leftTop = ImVec2{m_startPos.x, m_lastPos.y};
		m_size    = ImVec2{m_lastPos.x - m_startPos.x, m_startPos.y - m_lastPos.y};
	}
	//우상단->좌하단
	else if (m_lastPos.x - m_startPos.x < 0 && m_lastPos.y - m_startPos.y > 0)
	{
		m_leftTop = ImVec2{m_lastPos.x, m_startPos.y};
		m_size    = ImVec2{m_startPos.x - m_lastPos.x, m_lastPos.y - m_startPos.y};
	}
	//둘 중 한 쪽 축으로 이동한 것이므로 사이즈가 없다.
	else
	{
		m_bHasDragArea = false;
		m_leftTop      = ImVec2{0, 0};
		m_size         = ImVec2{0, 0};
	}

	//ImGui 윈도우 크기로 인한 좌표와 실제 이미지의 크기는 다르다.
	//따라서 실제 이미지의 좌표를 얻기 위해 비율을 구한다.
	m_leftTop /= windowSize;
	m_size /= windowSize;

	const Vec2 imageSize = {m_pFrameTexture->Width(), m_pFrameTexture->Height()};
	m_leftTop *= imageSize;
	m_size *= imageSize;
}

void UITool::ShowDefault(CUIBase* pScript) const
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(150.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::Button("[DEFAULT]");
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	ImGui::Text(u8"알파 표시");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	bool showFullRange = pScript->IsShowDebugRect();
	ImGui::Checkbox("##UI_SHOW_RANGE", &showFullRange);
	pScript->SetShowDebugRect(showFullRange);

	bool enableCollision = pScript->GetMouseCollision();
	ImGui::Text("MOUSE COLLISION");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	ImGui::Checkbox("##UI_COLLISION_EANBLE", &enableCollision);
	pScript->SetMouseCollision(enableCollision);

	ImGui::Text("OFFSET POS");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	Vec2 offsetPos = pScript->GetOffsetPos();
	ImGui::InputFloat2("##UI_OFFSET", &offsetPos.x);
	pScript->SetOffsetPos(offsetPos);

	Vec3 scale = pScript->GetOwner()->Transform()->GetRelativeScale();
	ImGui::Text("UI SIZE");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	ImGui::InputFloat2("##UI_SIZE", &scale.x);
	pScript->GetOwner()->Transform()->SetRelativeScale(scale);

	ImGui::Text("Z ORDER");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	int zOrder = pScript->GetOrderZ();
	if (ImGui::InputInt("##UI_Z_ORDER", &zOrder))
	{
		pScript->SetOrderZ(zOrder);
	}

	ImGui::Text("OPACITY");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	float opacity = pScript->GetOpacity();
	ImGui::InputFloat("##UI_OPACITY", &opacity);
	opacity = ClampData(opacity, 0.f, 1.f);
	pScript->SetOpacity(opacity);

	static const char* arrAlignH[] = {"LEFT", "MIDDLE", "RIGHT"};
	ImGui::Text("ALIGNMENT(HORIZONTAL)");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	if (ImGui::BeginCombo("##UI_HORIZONTAL_COMBO", arrAlignH[static_cast<UINT>(pScript->GetAnchorH())]))
	{
		for (size_t i = 0; i < std::size(arrAlignH); ++i)
		{
			if (ImGui::Selectable(arrAlignH[i]))
			{
				pScript->SetAnchorH(static_cast<ANCHOR_HORIZONTAL>(i));
			}
		}
		ImGui::EndCombo();
	}

	static const char* arrAlignV[] = {"TOP", "MIDDLE", "BOTTOM"};
	ImGui::Text("ALIGNMENT(VERTICAL)");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	if (ImGui::BeginCombo("##UI_VERTICAL_COMBO", arrAlignV[static_cast<UINT>(pScript->GetAnchorV())]))
	{
		for (size_t i = 0; i < std::size(arrAlignV); ++i)
		{
			if (ImGui::Selectable(arrAlignV[i]))
			{
				pScript->SetAnchorV(static_cast<ANCHOR_VERTICAL>(i));
			}
		}
		ImGui::EndCombo();
	}
}

void UITool::ShowPanel(CUIPanel* pScript)
{
	ShowDefault(pScript);
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(150.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::Button("[PANEL INFO]");
	ImGui::PopStyleColor(3);
	ImGui::PopID();


	CMeshRender*  pMeshRender        = pScript->GetOwner()->MeshRender();
	Ptr<CTexture> pTexture           = pMeshRender->GetMaterial(0)->GetTexParam(TEX_PARAM::TEX_0);
	bool          useDynamicMaterial = pMeshRender->IsUsingDynamicMaterial(0);

	ImGui::Text("USE DYNAMIC MATERIAL");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	if (ImGui::Checkbox("##DYNAMIC", &useDynamicMaterial))
	{
		pMeshRender->SetUseDynamicMaterial(0, useDynamicMaterial);
	}

	ImGui::Text("TEXTURE");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);

	ImGui::Text((nullptr != pTexture) ? ToString(pTexture->GetKey()).c_str() : "NO TEXTURE");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING + ImGui::GetItemRectSize().x);
	if (ImGui::Button("##TEXTURE_SELECT", ImVec2{15, 15}))
	{
		// ListUI 활성화한다.
		const auto& mapResource = CResMgr::GetInst()->GetResList(RES_TYPE::TEXTURE);
		const auto  pListUI     = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
		pListUI->Clear();
		pListUI->SetTitle("Texture List");
		for (const auto& [resourceKey, pResource] : mapResource)
		{
			pListUI->AddList(ToString(resourceKey));
		}

		pListUI->Activate();
		pListUI->SetDBCEvent(this, reinterpret_cast<DBCLKED>(&UITool::TextureSelected));
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset"))
	{
		pMeshRender->GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, nullptr);
	}
	ImGui::Text("USE WHOLE TEXTURE");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	static bool useWholeTexture = true;
	if (ImGui::Checkbox("##PANEL_USE_WHOLE", &useWholeTexture))
	{
		pScript->SetUseInfo(useWholeTexture ? false : true);
	}

	bool preserveRatio = pScript->IsPreserveRatio();
	ImGui::Text("PRESERVE RATIO");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	ImGui::Checkbox("##PANEL_PRESERVE_RATIO", &preserveRatio);
	pScript->SetPreserveRatio(preserveRatio);

	if (false == useWholeTexture)
	{
		ImGui::PushID(0);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(150.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
		ImGui::Button("[CURRENT INFO]");
		ImGui::PopStyleColor(3);
		ImGui::PopID();

		ImGui::Text("POS/SIZE");
		ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
		tImageInfo info = pScript->GetInfo();

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * .5f);
		if (ImGui::InputFloat2("##PANEL_INFO_POS", &info.imagePos.x))
		{
			pScript->SetImageInfo(info);
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::InputFloat2("##PANEL_INFO_SIZE", &info.imageSize.x))
		{
			pScript->SetImageInfo(info);
		}

		if (m_bHasDragArea && ImGui::Button("APPLY POS/SIZE##PANEL"))
		{
			pScript->SetImageInfo({Vec2{m_leftTop.x, m_leftTop.y}, Vec2{m_size.x, m_size.y}});
		}
	}
}

void UITool::ShowImage(CUIImage* pScript)
{
	ShowDefault(pScript);

	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(150.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::Button("[IMAGE INFO]");
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	CMeshRender*  pMeshRender = pScript->GetOwner()->MeshRender();
	Ptr<CTexture> pTexture    = pMeshRender->GetMaterial(0)->GetTexParam(TEX_PARAM::TEX_0);

	bool useDynamicMaterial = pMeshRender->IsUsingDynamicMaterial(0);
	ImGui::Text("USE DYNAMIC MATERIAL");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	if (ImGui::Checkbox("##DYNAMIC", &useDynamicMaterial))
	{
		pMeshRender->SetUseDynamicMaterial(0, useDynamicMaterial);
	}

	ImGui::Text("TEXTURE");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);

	ImGui::Text((nullptr != pTexture) ? ToString(pTexture->GetKey()).c_str() : "NO TEXTURE");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING + ImGui::GetItemRectSize().x);
	if (ImGui::Button("##TEXTURE_SELECT", ImVec2{15, 15}))
	{
		// ListUI 활성화한다.
		const auto& mapResource = CResMgr::GetInst()->GetResList(RES_TYPE::TEXTURE);
		const auto  pListUI     = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
		pListUI->Clear();
		pListUI->SetTitle("Texture List");
		for (const auto& [resourcKey, pResource] : mapResource)
		{
			pListUI->AddList(ToString(resourcKey));
		}

		pListUI->Activate();
		pListUI->SetDBCEvent(this, reinterpret_cast<DBCLKED>(&UITool::TextureSelected));
	}

	ImGui::SameLine();
	if (ImGui::Button("Reset"))
	{
		pMeshRender->GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, nullptr);
	}

	if (nullptr == pTexture)
	{
		return;
	}


	ImGui::Text("USE WHOLE TEXTURE");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	const bool  hasInfo = pScript->GetInfoList().empty();
	static bool useWholeTexture{};
	static bool useSelected{};
	useWholeTexture = hasInfo || useSelected;
	ImGui::Checkbox("##UI_USE_WHOLE", &useWholeTexture);

	bool preserveRatio = pScript->IsPreserveRatio();
	ImGui::Text("PRESERVE RATIO");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	ImGui::Checkbox("##PRESERVE_RATIO", &preserveRatio);
	pScript->SetPreserveRatio(preserveRatio);

	if (false == useWholeTexture)
	{
		if (pScript->HasCurrentInfo())
		{
			ImGui::PushID(0);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(150.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
			ImGui::Button("[CURRENT INFO]");
			ImGui::PopStyleColor(3);
			ImGui::PopID();

			ImGui::Text("NAME");
			ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);

			static char szName[256]{};
			_snprintf_s(szName, std::size(szName), "%s", pScript->CurrentInfoName().c_str());
			if (ImGui::InputText("##UI_CURRENT_NAME", szName, std::size(szName), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				const tImageInfo info = pScript->GetImageInfo(ToWString(pScript->CurrentInfoName()));
				pScript->RemoveInfo(ToWString(pScript->CurrentInfoName()));
				pScript->AddImageInfo(ToWString(szName), info.imagePos, info.imageSize);
				pScript->SetImageInfo(ToWString(szName));
			}

			ImGui::Text("POS/SIZE");
			ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
			tImageInfo info = pScript->GetImageInfo(ToWString(pScript->CurrentInfoName()));

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * .5f);
			if (ImGui::InputFloat2("##UI_INFO_POS", &info.imagePos.x))
			{
				pScript->AddImageInfo(ToWString(pScript->CurrentInfoName()), info.imagePos, info.imageSize);
			}
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::InputFloat2("##UI_INFO_SIZE", &info.imageSize.x))
			{
				pScript->AddImageInfo(ToWString(pScript->CurrentInfoName()), info.imagePos, info.imageSize);
			}

			if (m_bHasDragArea && ImGui::Button("APPLY POS/SIZE"))
			{
				pScript->AddImageInfo(ToWString(pScript->CurrentInfoName()),
				                      Vec2{m_leftTop.x, m_leftTop.y},
				                      Vec2{m_size.x, m_size.y});
				pScript->SetImageInfo(ToWString(pScript->CurrentInfoName()));
			}
		}

		ImGui::PushID(0);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(150.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
		ImGui::Button("[INFO LIST]");
		ImGui::PopStyleColor(3);
		ImGui::PopID();

		for (const auto& [frameName, info] : pScript->GetInfoList())
		{
			if (frameName == ToWString(pScript->CurrentInfoName()))
			{
				ImGui::TextColored({1.f, 0.f, 0.f, 1.f}, ToString(frameName).c_str());
				ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(),
				                                    ImGui::GetItemRectMax(),
				                                    IM_COL32(255, 0, 0, 255));
				ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);

				ImGui::TextColored({1.f, 0.f, 0.f, 1.f},
				                   "[POS] %.2f, %.2f [SIZE] %.2f, %.2f",
				                   info.imagePos.x,
				                   info.imagePos.y,
				                   info.imageSize.x,
				                   info.imageSize.y);
			}
			else
			{
				ImGui::Text(ToString(frameName).c_str());
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					pScript->SetImageInfo(frameName);
				}
				ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);

				ImGui::Text("[POS] %.2f, %.2f [SIZE] %.2f, %.2f",
				            info.imagePos.x,
				            info.imagePos.y,
				            info.imageSize.x,
				            info.imageSize.y);
			}
			ImGui::SameLine();
			if (ImGui::Button("UPDATE"))
			{
				pScript->AddImageInfo(frameName, Vec2{m_leftTop.x, m_leftTop.y}, Vec2{m_size.x, m_size.y});
			}
		}

		ImGui::BeginGroup();//BUTTON GROUP
		if (ImGui::Button("Add Frame"))
		{
			pScript->AddImageInfo(L"FRAME_" + std::to_wstring(pScript->GetInfoList().size()), Vec2{}, Vec2{});
		}

		ImGui::SameLine();
		if (ImGui::Button("Remove Selected Frame"))
		{
			pScript->RemoveInfo(ToWString(pScript->CurrentInfoName()));
		}
		ImGui::EndGroup();//BUTTON GROUP
	}
}

void UITool::ShowText(CUIText* pScript)
{
	ShowDefault(pScript);
	CMeshRender*  pMeshRender        = pScript->GetOwner()->MeshRender();
	Ptr<CTexture> pTexture           = pMeshRender->GetMaterial(0)->GetTexParam(TEX_PARAM::TEX_0);
	bool          useDynamicMaterial = pMeshRender->IsUsingDynamicMaterial(0);

	ImGui::Text("USE DYNAMIC MATERIAL");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	if (ImGui::Checkbox("##DYNAMIC", &useDynamicMaterial))
	{
		pMeshRender->SetUseDynamicMaterial(0, useDynamicMaterial);
	}

	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(150.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::Button("[TEXT INFO]");
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	ImGui::Text("FONT NAME");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	const std::string fontName = pScript->GetFontName().empty() ? "NO FONT" : pScript->GetFontName();
	ImGui::Text(fontName.c_str());
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING + ImGui::GetItemRectSize().x);
	if (ImGui::Button("##FONT_SELECT", ImVec2{15, 15}))
	{
		// ListUI 활성화한다.
		const auto& vecFont = CFontMgr::GetInst()->GetFonts();
		const auto  pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
		pListUI->Clear();
		pListUI->SetTitle("Font List");
		for (const auto& font : vecFont)
		{
			pListUI->AddList(ToString(font));
		}

		pListUI->Activate();
		pListUI->SetDBCEvent(this, reinterpret_cast<DBCLKED>(&UITool::FontSelected));
	}

	ImGui::Text("FONT SIZE");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	float fontSize = pScript->GetFontSize();
	ImGui::InputFloat("##UI_FONT_SIZE", &fontSize);
	pScript->SetFontSize(fontSize);

	ImGui::Text("FONT COLOR");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	Vec4 fontColor = pScript->GetColor();
	ImGui::ColorPicker3("##UI_FONT_COLOR", &fontColor.x);
	pScript->SetColor(fontColor);

	ImGui::Text("ENABLE ALPHA");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	bool alphaEanble = pScript->IsAlphaEnable();
	ImGui::Checkbox("##UI_TEXT_ALPHA", &alphaEanble);
	pScript->SetAlphaEnable(alphaEanble);

	static const char* arrAlignH[] = {"LEFT", "MIDDLE", "RIGHT"};
	ImGui::Text("TEXT ALIGNMENT(HORIZONTAL)");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	if (ImGui::BeginCombo("##UI_TEXT_ALIGN_HORIZONTAL", arrAlignH[static_cast<UINT>(pScript->GetAlignH())]))
	{
		for (size_t i = 0; i < std::size(arrAlignH); ++i)
		{
			if (ImGui::Selectable(arrAlignH[i]))
			{
				pScript->SetAlignH(static_cast<TEXT_ALIGN_HORIZONTAL>(i));
			}
		}
		ImGui::EndCombo();
	}

	static const char* arrAlignV[] = {"TOP", "MIDDLE", "BOTTOM"};
	ImGui::Text("TEXT ALIGNMENT(VERTICAL)");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	if (ImGui::BeginCombo("##UI_TEXT_ALIGN_VERTICAL", arrAlignV[static_cast<UINT>(pScript->GetAlignV())]))
	{
		for (size_t i = 0; i < std::size(arrAlignV); ++i)
		{
			if (ImGui::Selectable(arrAlignV[i]))
			{
				pScript->SetAlignV(static_cast<TEXT_ALIGN_VERTICAL>(i));
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Text(u8"시간 지정(이후 클리어됨)");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	bool enable = pScript->IsUsingPersistTime();
	if (ImGui::Checkbox("##USE_PERSIST_TIME", &enable))
	{
		pScript->UsePersistTime(enable);
	}

	if (enable)
	{
		ImGui::Text(u8"지속 시간");
		ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
		float persistTime = pScript->GetPersistTime();
		if (ImGui::InputFloat("##PERSIST_TIME", &persistTime))
		{
			pScript->SetPersistTime(persistTime);
		}
	}


	ImGui::Text(u8"텍스트");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);


	static char    szText[512]{};
	static wchar_t szwText[512]{};
	if (ImGui::InputTextMultiline("##INPUT_TEXT",
	                              szText,
	                              std::size(szText),
	                              {0, 0},
	                              ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
	{
		//IMGUI는 utf-8을 사용하므로 이를 wchar_t(유니코드16)으로 변경해줘야한다.
		//이건 약간 성능을 감수하고 플랫폼에 독립적으로 하려고 그냥 파일시스템으로 해본 것.
		//std::wstring text = std::filesystem::u8path(szText);
		int nLen = MultiByteToWideChar(CP_UTF8, 0, szText, (int)strlen(szText), nullptr, 0);
		MultiByteToWideChar(CP_UTF8, 0, szText, (int)strlen(szText), szwText, nLen);
		pScript->SetText(szwText);
		std::memset(szText, 0, std::size(szText));
		std::memset(szwText, 0, std::size(szwText));
	}
}

void UITool::ShowButton(CUIButton* pScript) const
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(150.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::Button("[BUTTON SCRIPT INFO]");
	ImGui::PopStyleColor(3);
	ImGui::PopID();
}

void UITool::ShowProgressBar(CUIProgressBar* pScript)
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(220.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(150.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f));
	ImGui::Button("[PROGRESS BAR SCRIPT INFO]");
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	ImGui::Text("PERCENTAGE");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	float percent = pScript->GetPercent();
	if (ImGui::DragFloat("##INPUT_PERCENT", &percent, 0.1f, 0.f, 1.f))
	{
		pScript->SetPercent(percent);
	}
	ImGui::Text("DIRECTION");
	ImGui::SameLine(UI_TOOL::DEFAULT_ITEM_SPACING);
	static const char* arrProgressDir[] = {"RIGHT_TO_LEFT", "LEFT_TO_RIGHT", "TOP_TO_BOTTOM", "BOTTOM_TO_TOP"};
	if (ImGui::BeginCombo("##UI_PROGRESS_BAR_COMBO", arrProgressDir[static_cast<UINT>(pScript->GetDir())]))
	{
		for (size_t i = 0; i < std::size(arrProgressDir); ++i)
		{
			if (ImGui::Selectable(arrProgressDir[i]))
			{
				pScript->SetDir(static_cast<PROGRESS_DIR>(i));
			}
		}
		ImGui::EndCombo();
	}
}

void UITool::LoadUIPrefab()
{
	wchar_t szName[256]{};

	OPENFILENAME ofn{};
	ofn.lStructSize             = sizeof(OPENFILENAME);
	ofn.hwndOwner               = CCore::GetInst()->GetMainHwnd();
	ofn.lpstrFile               = szName;
	ofn.nMaxFile                = sizeof(szName);
	ofn.lpstrFilter             = L"Prefab\0*.pref\0";
	ofn.nFilterIndex            = 0;
	ofn.lpstrFileTitle          = nullptr;
	ofn.nMaxFileTitle           = 0;
	ofn.Flags                   = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	const wstring strTileFolder = CPathMgr::GetInst()->GetContentPath() + L"prefab";
	ofn.lpstrInitialDir         = strTileFolder.c_str();

	if (GetOpenFileName(&ofn))
	{
		const std::wstring relativePath = CPathMgr::GetInst()->GetRelativePath(szName);
		Ptr<CPrefab>       pPrefab      = CResMgr::GetInst()->FindRes<CPrefab>(relativePath);
		assert(nullptr != pPrefab);

		const CUIBase* pUIComponent = pPrefab->GetProto()->GetUIBaseComponenent();
		if (nullptr == pUIComponent)
		{
			m_bShowWarning      = true;
			m_bWrongPrefab      = true;
			m_bOpenWarningPopUp = true;
		}
		else
		{
			CGameObject* pGameObject = pPrefab->Instantiate();
			CSceneMgr::GetInst()->SpawnObject(pGameObject, UI_TOOL::UI_LAYER);
		}
	}
}

void UITool::SaveAsPrefab()
{
	wchar_t szName[256]{};

	OPENFILENAME ofn{};
	ofn.lStructSize             = sizeof(OPENFILENAME);
	ofn.hwndOwner               = CCore::GetInst()->GetMainHwnd();
	ofn.lpstrFile               = szName;
	ofn.nMaxFile                = sizeof(szName);
	ofn.lpstrFilter             = L"Prefab\0*.pref\0";
	ofn.nFilterIndex            = 0;
	ofn.lpstrFileTitle          = nullptr;
	ofn.nMaxFileTitle           = 0;
	const wstring strTileFolder = CPathMgr::GetInst()->GetContentPath() + L"prefab";
	ofn.lpstrInitialDir         = strTileFolder.c_str();
	ofn.Flags                   = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
	{
		const std::wstring relativePath = CPathMgr::GetInst()->GetRelativePath(szName);
		Ptr<CPrefab>       pPrefab      = CResMgr::GetInst()->FindRes<CPrefab>(relativePath);

		//이미 있는 프리팹을 선택하였으므로, 해당 프리팹을 강제 교체.
		//단 UI Prefab이 아닌 경우 선택 실패
		if (nullptr != pPrefab)
		{
			const CUIBase* pUIComponent = pPrefab->GetProto()->GetUIBaseComponenent();
			if (nullptr == pUIComponent)
			{
				m_bShowWarning      = true;
				m_bWrongPrefab      = true;
				m_bOpenWarningPopUp = true;
			}
			else
			{
				//메모리에서 삭제
				CResMgr::GetInst()->ForceDeleteRes<CPrefab>(pPrefab->GetKey());
				//파일 자체를 삭제
				std::filesystem::remove(szName);
			}
		}
		//ToolScene의 루트를 프리팹화
		pPrefab = new CPrefab{};
		pPrefab->SetProto(m_pSelected->Clone());
		pPrefab->Save(szName);
		CResMgr::GetInst()->AddRes<CPrefab>(relativePath, pPrefab.Get());
	}
}
