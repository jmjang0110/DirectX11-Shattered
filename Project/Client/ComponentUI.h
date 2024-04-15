#pragma once
#include "UI.h"

#include <Engine/CGameObject.h>

class ComponentUI : public UI
{
private:
	CGameObject* m_pTargetObject;
	COMPONENT_TYPE m_eComType;


	bool			m_bActive;
	bool            m_bDelete;

	// todo - Component 구성 요소들을 접는 토글 
	bool			m_bFold;

public:
	void         SetTargetObject(CGameObject* _pTarget) { m_pTargetObject = _pTarget; }
	CGameObject* GetTargetObject() { return m_pTargetObject; }

protected:
	bool IsComponentActive() { return m_bActive; }
	bool IsFold() { return m_bFold; }

public:
	virtual void update() override;
	virtual void render_update() override;

public:
	ComponentUI(const string& _strName, COMPONENT_TYPE _eComType);
	virtual ~ComponentUI();
};
