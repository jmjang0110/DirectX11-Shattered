#pragma once
#include "ComponentUI.h"

class NaviMapUI : public ComponentUI
{
private:
	virtual void render_update() override;

public:
	void NaviMapDataSelect(DWORD_PTR _param);

	NaviMapUI();
	virtual ~NaviMapUI();
};
