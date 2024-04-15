#include "pch.h"
#include "ScriptUI.h"

#include <Engine/CScript.h>
#include <Script/CScriptMgr.h>

#include "ParamUI.h"
#include "IconsFontAwesome5.h"
#include "InspectorUI.h"
#include "CImGuiMgr.h"
#include <Engine/CResMgr.h>
#include <Engine/CTexture.h>

ScriptUI::ScriptUI()
	: UI("Script")
	, m_pTargetObject(nullptr)
	, m_pTargetScript(nullptr)
	, m_eComType(COMPONENT_TYPE::SCRIPT)
	, m_bActive(false)
	, m_bFold{false}
	, m_bTextureSelected{false} {}

ScriptUI::~ScriptUI() = default;

void ScriptUI::update() {}

void ScriptUI::render_update()
{
	RenderButton();
	RenderDeleteWarning();
	RenderParamInfo();
}

void ScriptUI::RenderButton()
{
	// 담당 Script 이름
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(50.f / 255.f, 170.f / 255.f, 50.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(50.f / 255.f, 170.f / 255.f, 50.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(50.f / 255.f, 120.f / 255.f, 50.f / 255.f, 1.f));

	m_strScriptName = ToString(CScriptMgr::GetScriptName(m_pTargetScript));

	if (m_bFold == false)
	{
		m_strButtonName = ICON_FA_CARET_DOWN;
	}
	else
	{
		m_strButtonName = ICON_FA_CARET_RIGHT;
	}
	m_strButtonName += m_strScriptName;
	if (ImGui::Button(m_strButtonName.c_str()))
	{
		m_bFold = !m_bFold;
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();


	// SCRIPT 활성화 체크
	if (m_pTargetScript != nullptr)
	{
		bool isActive = m_pTargetScript->IsActive();
		ImGui::SameLine();

		if (isActive)
		{
			ImGui::Checkbox(ICON_FA_EYE, &isActive);
		}
		else
		{
			ImGui::Checkbox(ICON_FA_EYE_SLASH, &isActive);
		}

		if (m_pTargetScript->IsActive() != isActive)
		{
			if (isActive)
			{
				m_pTargetScript->Activate();
			}
			else
			{
				m_pTargetScript->Deactivate();
			}
		}
	}
}

void ScriptUI::RenderDeleteWarning()
{
	// [ SCRIPT DELETE BUTTON ]
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_TRASH))
	{
		ImGui::OpenPopup(u8"SCRIPT 삭제 경고창");
	}

	const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(u8"SCRIPT 삭제 경고창", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		const string targetObjectName = ToString(m_pTargetObject->GetName());
		const string text             = targetObjectName + " / " + m_strScriptName;
		ImGui::Text(text.c_str());
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), u8"해당 SCRIPT를 정말로 삭제하시겠습니까?\n\n");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			tUIDelegate tDeleteCom{};
			tDeleteCom.dwParam = (DWORD_PTR)m_pTargetScript;
			tDeleteCom.pFunc   = static_cast<PARAM_1>(&InspectorUI::DeleteScript);
			tDeleteCom.pInst   = CImGuiMgr::GetInst()->FindUI("Inspector");

			CImGuiMgr::GetInst()->AddDelegate(tDeleteCom);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

void ScriptUI::RenderParamInfo()
{
	const vector<tScriptParamInfo>& vecParam = m_pTargetScript->GetScriptParam();
	for (const tScriptParamInfo& paramInfo : vecParam)
	{
		switch (paramInfo.showType)
		{
		case SHOW_TYPE::NORMAL:
			RenderParamNormal(paramInfo);
			break;
		case SHOW_TYPE::DROP_DOWN:
			RenderParamDropDown(paramInfo);
			break;
		case SHOW_TYPE::CHECK_BOX:
			RenderParamCheckBox(paramInfo);
			break;
		case SHOW_TYPE::COLOR_PICKER:
			RenderParamColorPicker(paramInfo);
			break;
		case SHOW_TYPE::END:
			assert(nullptr && "INVALID SCRIPT INFO");
			break;
		default:
			assert(nullptr && "INVALID SCRIPT INFO");
		}
	}
}

void ScriptUI::RenderParamNormal(const tScriptParamInfo& info)
{
	switch (info.eType)
	{
	case SCRIPTPARAM_TYPE::INT:
		{
			const int beforeChange = *static_cast<const int*>(info.pParam);
			const int data = ParamUI::Param_Int(info.strParamName, static_cast<const int*>(info.pParam));
			*static_cast<int*>(info.pParam) = data;
			if (beforeChange != data && info.callback)
			{
				info.callback();
			}
			break;
		}
	case SCRIPTPARAM_TYPE::FLOAT:
		{
			const float beforeChange = *static_cast<const float*>(info.pParam);
			const float data         = ParamUI::Param_Float(info.strParamName,
			                                                static_cast<const float*>(info.pParam));
			*static_cast<float*>(info.pParam) = data;
			if (false == CloseEnough(beforeChange, data) && info.callback)
			{
				info.callback();
			}
			break;
		}
	case SCRIPTPARAM_TYPE::VEC2:
		{
			const Vec2 beforeChange = *static_cast<const Vec2*>(info.pParam);
			const Vec2 data = ParamUI::Param_Vec2(info.strParamName, static_cast<const Vec2*>(info.pParam));
			*static_cast<Vec2*>(info.pParam) = data;
			if (beforeChange != data && info.callback)
			{
				info.callback();
			}
			break;
		}
	case SCRIPTPARAM_TYPE::VEC4:
		{
			const Vec4 beforeChange = *static_cast<const Vec4*>(info.pParam);
			const Vec4 data = ParamUI::Param_Vec4(info.strParamName, static_cast<const Vec4*>(info.pParam));
			*static_cast<Vec4*>(info.pParam) = data;
			if (beforeChange != data && info.callback)
			{
				info.callback();
			}
			break;
		}
	case SCRIPTPARAM_TYPE::TEXT:
		{
			const std::string beforeChange = static_cast<std::string*>(info.pParam)->c_str();
			const std::string result       = ParamUI::Param_String(info.strParamName,
			                                                       static_cast<std::string*>(info.pParam)->
			                                                       c_str());
			if (result.empty())
			{
				break;
			}
			std::string* ptr = static_cast<std::string*>(info.pParam);
			ptr->clear();
			ptr->resize(result.size());
			std::memcpy(ptr->data(), result.data(), result.size() * sizeof(char));
			if (beforeChange != result && info.callback)
			{
				info.callback();
			}
			break;
		}
	case SCRIPTPARAM_TYPE::TEXT_READONLY:
		{
			ParamUI::Param_String_ReadOnly(info.strParamName, static_cast<std::string*>(info.pParam)->c_str());
			if (info.callback)
			{
				info.callback();
			}
			break;
		}
	case SCRIPTPARAM_TYPE::TEXTURE:
		{
			if (false == m_bTextureSelected)
			{
				m_textureName.clear();
			}
			ParamUI::Param_Tex("TextureName",
			                   CResMgr::GetInst()->FindRes<
				                   CTexture>(ToWString(static_cast<std::string*>(info.pParam)->c_str())).Get(),
			                   this,
			                   (DBCLKED)&ScriptUI::SelectTexture);
			if (m_textureName.empty())
			{
				return;
			}
			std::string* ptr = static_cast<std::string*>(info.pParam);
			ptr->clear();
			ptr->resize(m_textureName.size());
			std::memcpy(ptr->data(), m_textureName.data(), m_textureName.size() * sizeof(char));
			m_bTextureSelected = false;

			break;
		}
	case SCRIPTPARAM_TYPE::PREFAB:
		break;
	}
}

void ScriptUI::RenderParamDropDown(const tScriptParamInfo& info)
{
	switch (info.eType)
	{
	case SCRIPTPARAM_TYPE::INT:
		{
			const int beforeChange = *static_cast<const int*>(info.pParam);
			const int data         = ParamUI::Param_Int_DropDown(info.strParamName,
			                                                     static_cast<const int*>(info.pParam),
			                                                     info.vecKind);
			*static_cast<int*>(info.pParam) = data;
			if (beforeChange != data && info.callback)
			{
				info.callback();
			}
			break;
		}
	case SCRIPTPARAM_TYPE::FLOAT:
		{
			break;
		}
	case SCRIPTPARAM_TYPE::VEC2:
		{
			break;
		}
	case SCRIPTPARAM_TYPE::VEC4:
		{
			break;
		}
	default:
		{
			assert(nullptr && "INVALID SCRIPTPARAM_TYPE, OR CANNOT INC/DEC");
		}
	}
}

void ScriptUI::RenderParamCheckBox(const tScriptParamInfo& info)
{
	switch (info.eType)
	{
	case SCRIPTPARAM_TYPE::INT:
		{
			const int beforeChange = *static_cast<const int*>(info.pParam);
			const int data = ParamUI::Param_Int_CheckBox(info.strParamName, static_cast<const int*>(info.pParam));
			*static_cast<int*>(info.pParam) = data;
			if (beforeChange != data && info.callback)
			{
				info.callback();
			}
			break;
		}
	default:
		{
			assert(nullptr && "INVALID SCRIPTPARAM_TYPE, OR CANNOT INC/DEC");
		}
	}
}

void ScriptUI::RenderParamColorPicker(const tScriptParamInfo& info)
{
	switch (info.eType)
	{
	case SCRIPTPARAM_TYPE::VEC4:
		{
			const Vec4 beforeChange = *static_cast<const Vec4*>(info.pParam);
			const Vec4 data = ParamUI::Param_Vec4_ColorPicker(info.strParamName, static_cast<const Vec4*>(info.pParam));
			*static_cast<Vec4*>(info.pParam) = data;
			if (beforeChange != data && info.callback)
			{
				info.callback();
			}
			break;
		}
	default:
		{
			assert(nullptr && "INVALID SCRIPTPARAM_TYPE, OR NOT VEC4");
		}
	}
}

void ScriptUI::SelectTexture(DWORD_PTR _pTextureName)
{
	m_textureName      = (const char*)_pTextureName;
	m_bTextureSelected = true;
}
