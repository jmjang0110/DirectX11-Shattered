#pragma once
#include "ComponentUI.h"

class GameButtonUI final
	: public ComponentUI
{
public:
	void render_update() override;

public:
	GameButtonUI();
	virtual ~GameButtonUI();
};
