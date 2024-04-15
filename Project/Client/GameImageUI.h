#pragma once
#include "ComponentUI.h"

class GameImageUI final
	: public ComponentUI
{
public:
	void render_update() override;

public:
	GameImageUI();
	virtual ~GameImageUI();
};
