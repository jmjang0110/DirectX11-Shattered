#pragma once
#include <Engine/CScript.h>

class CameraMoveScript : public CScript
{
private:
	CGameObject* m_Filter;
	float        m_fCamSpeed;

public:
	void SetFilter(CGameObject* _pFilterObject) { m_Filter = _pFilterObject; }

public:
	virtual void update() override;
	CLONE(CameraMoveScript)
public:
	CameraMoveScript();
	virtual ~CameraMoveScript();
};
