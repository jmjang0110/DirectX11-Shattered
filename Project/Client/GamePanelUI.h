#pragma once
#include "ComponentUI.h"

class GamePanelUI final
	: public ComponentUI
{
public:
	void render_update() override;

public:
	GamePanelUI();
	virtual ~GamePanelUI();
};
