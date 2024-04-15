#pragma once
#include "ComponentUI.h"

class NaviAgentUI : public ComponentUI
{
private:
	virtual void render_update() override;

public:
	NaviAgentUI();
	virtual ~NaviAgentUI();
};
