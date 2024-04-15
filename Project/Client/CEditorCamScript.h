#pragma once
#include <Engine/CScript.h>

enum class PROJ_TYPE;

class CEditorCamScript : public CScript
{
private:
	float     m_fSpeed;
	bool      m_bEnableInput = true;
	PROJ_TYPE m_projectionType;
public:
	virtual void update() override;


	CLONE(CEditorCamScript);
public:
	CEditorCamScript();
	virtual ~CEditorCamScript();
};
