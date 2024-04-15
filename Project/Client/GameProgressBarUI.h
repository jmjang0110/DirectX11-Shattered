#pragma once
#include "ComponentUI.h"

class GameProgressBarUI final
	: public ComponentUI
{
public:
	void render_update() override;

public:
	GameProgressBarUI();
	virtual ~GameProgressBarUI();
};
