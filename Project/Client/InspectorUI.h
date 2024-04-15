#pragma once
#include "UI.h"

#include <Engine/CGameObject.h>

class ComponentUI;
class ScriptUI;
class ResInfoUI;
class CRes;


class InspectorUI : public UI
{
private:
	CScene* m_pTargetScene;
	CLayer* m_pTargetLayer;
	CGameObject* m_pTargetObject;
	CRes* m_pTargetRes;

	ComponentUI* m_arrComUI[(UINT)COMPONENT_TYPE::END];
	ResInfoUI* m_arrResUI[(UINT)RES_TYPE::END];

	vector<ScriptUI*>	m_vecScriptUI;

public:
	// SET PART 
	void SetTargetObject(CGameObject* _pTarget);
	void SetTargetScene(CScene* _scene) { m_pTargetScene = _scene; }
	void SetTargetLayer(CLayer* _layer) { m_pTargetLayer = _layer; }
	void SetTargetResource(CRes* _pTargetRes);

	// GET PART 
	CGameObject* GetTargetObject() { return m_pTargetObject; }
	CScene* GetTargetScene() { return m_pTargetScene; }
	CLayer* GetTargetLayer() { return m_pTargetLayer; }



private:
	ScriptUI* AddScriptUI();
	void RenderButton();

public:
	// DELEGATE FUNC
	void AddComponent(DWORD_PTR _param);
	void AddScript(DWORD_PTR _param);

	void DeleteComponent(DWORD_PTR _param);
	void DeleteScript(DWORD_PTR _param);


public:
	virtual void update() override;
	virtual void render_update() override;


public:
	InspectorUI();
	virtual ~InspectorUI();
};
