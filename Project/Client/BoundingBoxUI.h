#pragma once
#include "ComponentUI.h"
class BoundingBoxUI :
	public ComponentUI
{
private:
	virtual void render_update() override;

public:
	BoundingBoxUI();
	virtual ~BoundingBoxUI();

};

