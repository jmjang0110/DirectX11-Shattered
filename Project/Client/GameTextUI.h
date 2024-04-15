#pragma once
#include "ComponentUI.h"

class GameTextUI final
	: public ComponentUI
{
public:
	void render_update() override;

public:
	GameTextUI();
	virtual ~GameTextUI();
};
