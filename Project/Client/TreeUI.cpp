#include "pch.h"
#include "TreeUI.h"

#include "ImGui/imgui_internal.h"

#include "SceneOutliner.h"
#include "CImGuiMgr.h"
#include "InspectorUI.h"

#include <Engine/CScene.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>

#include "IconsFontAwesome5.h"

// ========
// TreeNode
// ========
TreeNode::TreeNode()
	: m_pTreeUI(nullptr)
	, m_pParent(nullptr)
	, m_bLeaf(true)
	, m_bSelected(false)
	, m_dwData(0)
	, m_eNodeType(NODE_TYPE::NONE)
	, m_bCheckOn(false) {}

TreeNode::TreeNode(const string& _strName, DWORD_PTR _dwData)
	: m_pTreeUI(nullptr)
	, m_pParent(nullptr)
	, m_bLeaf(true)
	, m_bSelected(false)
	, m_strName(_strName)
	, m_dwData(_dwData) {}

TreeNode::~TreeNode()
{
	Safe_Del_Vec(m_vecChild);
}


void TreeNode::RenderSelectedMark(ImU32 _color)
{
	if (m_eNodeType == NODE_TYPE::ENGINE_RESOURCE)
		return;

	auto pInspectorUI = static_cast<InspectorUI*>(CImGuiMgr::GetInst()->FindUI("Inspector"));
	// [ 원 - 출력 ] / [원본] - ImGui::Bullet()
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiContext&     g           = *GImGui;
	const ImGuiStyle& style       = g.Style;
	const float       line_height = ImMax(ImMin(window->DC.CurrLineSize.y, g.FontSize + style.FramePadding.y * 2),
	                                      g.FontSize);
	const ImRect bb(window->DC.CursorPos,
	                ImVec2(window->DC.CursorPos.x + g.FontSize, window->DC.CursorPos.y + line_height));
	ImGui::ItemSize(bb);
	if (!ImGui::ItemAdd(bb, 0))
	{
		ImGui::SameLine(0, style.FramePadding.x * 0.5f);
		return;
	}
	ImU32 text_col = _color;
	ImGui::RenderBullet(window->DrawList,
	                    ImVec2(bb.Min.x + style.FramePadding.x + g.FontSize * 0.5f, bb.Min.y + line_height * 0.5f),
	                    text_col);
	ImGui::SameLine(0, style.FramePadding.x * 0.5f);

	// [ 파일 아이콘 - 출력 ]
	if ((m_dwData == (DWORD_PTR)pInspectorUI->GetTargetScene() ||
	     m_dwData == (DWORD_PTR)pInspectorUI->GetTargetLayer()))
	{
		ImGui::Text("%s", ICON_FA_FOLDER_OPEN);
		ImGui::SameLine(0, style.FramePadding.x * 0.5f);
	}
	else if (m_dwData == (DWORD_PTR)pInspectorUI->GetTargetObject())
	{
		ImGui::Text("%s", ICON_FA_FILE_IMPORT);
		ImGui::SameLine(0, style.FramePadding.x * 0.5f);
	}

	// [ 레이어 인덱스 - 출력 ]
	if (NODE_TYPE::ENGINE_LAYER == m_eNodeType &&
	    m_dwData == (DWORD_PTR)pInspectorUI->GetTargetLayer())
	{
		auto pLayer   = (CLayer*)m_dwData;
		int  LayerIdx = pLayer->GetLayerIdx();
		auto color    = ImVec4(204.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.8f);
		ImGui::TextColored(color, std::to_string(LayerIdx).c_str());
		ImGui::SameLine(0, style.FramePadding.x * 0.5f);
	}
	else if (NODE_TYPE::ENGINE_LAYER == m_eNodeType &&
	         m_dwData != (DWORD_PTR)pInspectorUI->GetTargetLayer())
	{
		auto pLayer   = (CLayer*)m_dwData;
		int  LayerIdx = pLayer->GetLayerIdx();
		auto color    = ImVec4(200.f, 200.f, 200.f, 255.f);
		ImGui::TextColored(color, std::to_string(LayerIdx).c_str());
		ImGui::SameLine(0, style.FramePadding.x * 0.5f);
	}
}

void TreeNode::update()
{
	// update 작성


	// child update
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->update();
	}
}

void TreeNode::render_update()
{
	// ImGuiTreeNodeFlags_	
	ImGuiTreeNodeFlags eFlag = 0;

	if (m_bLeaf)
		eFlag |= ImGuiTreeNodeFlags_Leaf;
	if (m_pTreeUI->m_bUseFrame && (nullptr == m_pParent || m_pParent->m_strName == "DummyRoot"))
		eFlag |= ImGuiTreeNodeFlags_Framed;
	if (m_bSelected)
		eFlag |= ImGuiTreeNodeFlags_Selected;


	// ==================================== Todo =========================================
	auto pInspectorUI = static_cast<InspectorUI*>(CImGuiMgr::GetInst()->FindUI("Inspector"));
	if ((m_dwData == (DWORD_PTR)pInspectorUI->GetTargetScene() ||
	     m_dwData == (DWORD_PTR)pInspectorUI->GetTargetLayer() ||
	     m_dwData == (DWORD_PTR)pInspectorUI->GetTargetObject()))
		RenderSelectedMark(IM_COL32(102.f, 255.f, 255.f, 255.f));
	else
		RenderSelectedMark(IM_COL32(70.f, 70.f, 70.f, 255.f));
	// ===================================================================================


	if (m_bCheckOn)
	{
		ImGui::SetNextItemOpen(true);
	}
	if (ImGui::TreeNodeEx(m_strName.c_str(), eFlag))
	{
		m_bCheckOn = false;

		// 노드의 클릭체크
		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			m_pTreeUI->SetSelectedNode(this);
		}

		// 노드의 더블클릭체크
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			m_pTreeUI->SetDBClickedNode(this);
		}

		// 내부 또는 외부로 드래그 드롭을 사용 할 경우
		if (m_pTreeUI->m_bUseDragDropSelf || m_pTreeUI->m_bUseDragDropOuter)
		{
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload(m_pTreeUI->GetParentUI()->GetName().c_str(), &m_dwData, sizeof(DWORD_PTR));
				ImGui::Text(m_strName.c_str());
				ImGui::EndDragDropSource();

				m_pTreeUI->m_pDragNode = this;
			}

			// 내부 드롭을 허용한 경우에만 드롭체크
			if (m_pTreeUI->m_bUseDragDropSelf)
			{
				if (ImGui::BeginDragDropTarget())
				{
					DWORD_PTR           dwData  = 0;
					const ImGuiPayload* payload =
						ImGui::AcceptDragDropPayload(m_pTreeUI->GetParentUI()->GetName().c_str());
					if (nullptr != payload)
					{
						memcpy(&dwData, payload->Data, sizeof(DWORD_PTR));

						// Drop Node 를 알림
						m_pTreeUI->m_pDropNode = this;
					}

					ImGui::EndDragDropTarget();
				}
			}
		}

		for (size_t i = 0; i < m_vecChild.size(); ++i)
		{
			m_vecChild[i]->render_update();
		}

		ImGui::TreePop();
	}
}


// ======
// TreeUI
// ======
TreeUI::TreeUI(bool _bDummyRoot)
	: UI("##TreeUI")
	, m_pRootNode(nullptr)
	, m_pSelectedNode(nullptr)
	, m_pDragNode(nullptr)
	, m_pDropNode(nullptr)
	, m_bUseDummyRoot(_bDummyRoot)
	, m_bShowDummy(false)
	, m_bUseFrame(false)
	, m_bUseDragDropSelf(false)
	, m_bUseDragDropOuter(false)
	, m_pCInst(nullptr)
	, m_CFunc(nullptr)
	, m_pDBCInst(nullptr)
	, m_DBCFunc(nullptr)
	, m_pDADInst(nullptr)
	, m_DADFunc(nullptr)
{
	if (m_bUseDummyRoot)
	{
		AddTreeNode(nullptr, "DummyRoot");
	}
}

TreeUI::~TreeUI()
{
	SAFE_DELETE(m_pRootNode);
}

void TreeUI::update()
{
	if (nullptr == m_pRootNode)
	{
		m_pSelectedNode = nullptr;
		return;
	}

	// 트리UI 가 부착된 부모 UI 의 사이즈를 받아온다.
	Vec2 vSize = GetParentUI()->GetSize();
	vSize.y -= 39.f;
	SetSize(vSize);

	m_pRootNode->update();


	// 자식 UI 업데이트
	UI::update();
}

void TreeUI::render_update()
{
	if (nullptr == m_pRootNode)
		return;

	if (m_bUseDummyRoot)
	{
		if (m_bShowDummy)
			m_pRootNode->render_update();
		else
		{
			for (size_t i = 0; i < m_pRootNode->m_vecChild.size(); ++i)
			{
				m_pRootNode->m_vecChild[i]->render_update();
			}
		}
	}

	else
	{
		m_pRootNode->render_update();
	}

	// Drag Drop Check
	if ((m_pDragNode && m_pDropNode)
	    || m_pDragNode && KEY_AWAY(KEY::LBTN))
	{
		if (m_pDADInst && m_DADFunc)
		{
			if (nullptr == m_pDropNode)
			{
				(m_pDADInst->*m_DADFunc)(m_pDragNode->GetData(), 0);
			}
			else
			{
				(m_pDADInst->*m_DADFunc)(m_pDragNode->GetData(), m_pDropNode->GetData());
			}
		}
		m_pDragNode = nullptr;
		m_pDropNode = nullptr;
	}


	// KeyBinding 호출
	if (ImGui::IsWindowFocused())
	{
		for (size_t i = 0; i < m_vecKeyBind.size(); ++i)
		{
			if (KEY_TAP(m_vecKeyBind[i].eKey))
			{
				(m_vecKeyBind[i].pInst->*m_vecKeyBind[i].pFunc)((DWORD_PTR)m_pSelectedNode);
			}
		}
	}
}

TreeNode* TreeUI::AddTreeNode(TreeNode* _pParentNode, const string& _strName, DWORD_PTR _dwData)
{
	auto pNewNode       = new TreeNode(_strName, _dwData);
	pNewNode->m_pTreeUI = this;

	// 부모를 지정함
	if (nullptr != _pParentNode)
	{
		_pParentNode->AddChild(pNewNode);
	}
	// 부모가 지정되지 않고
	else if (nullptr == _pParentNode)
	{
		// 루트노드가 존재함
		if (nullptr != m_pRootNode)
			m_pRootNode->AddChild(pNewNode);

			// 루트노드가 존재하지 않음
		else
			m_pRootNode = pNewNode;
	}

	return pNewNode;
}


void TreeUI::SetKeyBinding(KEY _eKey, UI* _pInst, KEY_FUNC _Func)
{
	m_vecKeyBind.push_back(tTreeKey{_eKey, _pInst, _Func});
}

void TreeUI::Clear()
{
	SAFE_DELETE(m_pRootNode);
	m_pRootNode     = nullptr;
	m_pSelectedNode = nullptr;

	if (m_bUseDummyRoot)
	{
		AddTreeNode(nullptr, "DummyRoot");
	}
}

#include <queue>

TreeNode* TreeUI::FindNode(const std::string& _nodeName)
{
	std::queue<TreeNode*> queueSearch{};

	queueSearch.push(m_pRootNode);

	while (false == queueSearch.empty())
	{
		TreeNode* pNode = queueSearch.front();
		queueSearch.pop();

		if (pNode->m_strName == _nodeName)
		{
			return pNode;
		}

		for (auto& pChildNode : pNode->m_vecChild)
		{
			queueSearch.push(pChildNode);
		}
	}
	return nullptr;
}

void TreeUI::SetSelectedNode(TreeNode* _pNode)
{
	if (nullptr != m_pSelectedNode)
	{
		m_pSelectedNode->m_bSelected = false;
	}

	m_pSelectedNode              = _pNode;
	m_pSelectedNode->m_bSelected = true;


	// 델리게이트 호출
	if (nullptr != m_pCInst && nullptr != m_CFunc)
	{
		(m_pCInst->*m_CFunc)((DWORD_PTR)m_pSelectedNode);
	}
}

void TreeUI::SetDBClickedNode(TreeNode* _pNode)
{
	// 델리게이트 호출
	if (nullptr != m_pDBCInst && nullptr != m_DBCFunc)
	{
		(m_pDBCInst->*m_DBCFunc)((DWORD_PTR)m_pSelectedNode);
	}
}


void TreeUI::DropCheck()
{
	if (ImGui::BeginDragDropTarget())
	{
		DWORD_PTR dwData = 0;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetAcceptDragUIName().c_str()))
		{
			memcpy(&dwData, payload->Data, sizeof(DWORD_PTR));

			if (m_pDropInstance && m_dropFuc)
			{
				(m_pDropInstance->*m_dropFuc)(dwData);
			}
		}

		ImGui::EndDragDropTarget();
	}
}
