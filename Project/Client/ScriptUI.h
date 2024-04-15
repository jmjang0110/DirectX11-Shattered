#pragma once
#include "UI.h"

#include <Engine/CGameObject.h>

struct tScriptParamInfo;

class ScriptUI
	: public UI
{
private:
	CGameObject*   m_pTargetObject;
	CScript*       m_pTargetScript;
	COMPONENT_TYPE m_eComType;

	bool m_bActive;
	bool m_bFold;

	std::string m_strButtonName;
	std::string m_strScriptName;
	std::string m_textureName;
	bool        m_bTextureSelected;
private:
	void RenderButton();
	void RenderDeleteWarning();

	void RenderParamInfo();
	void RenderParamNormal(const tScriptParamInfo& info);
	void RenderParamDropDown(const tScriptParamInfo& info);
	void RenderParamCheckBox(const tScriptParamInfo& info);
	void RenderParamColorPicker(const tScriptParamInfo& info);

	void SelectTexture(DWORD_PTR _pTextureName);
protected:
	bool IsComponentActive() const { return m_bActive; }
	bool IsFold() const { return m_bFold; }
public:
	void         SetTargetObject(CGameObject* _pTarget) { m_pTargetObject = _pTarget; }
	CGameObject* GetTargetObject() const { return m_pTargetObject; }

	void     SetTargetScript(CScript* _pScript) { m_pTargetScript = _pScript; }
	CScript* GetTargetScript() const { return m_pTargetScript; }

public:
	void update() override;
	void render_update() override;

public:
	ScriptUI();
	virtual ~ScriptUI();
};
