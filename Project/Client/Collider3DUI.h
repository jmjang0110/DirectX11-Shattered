#pragma once
#include "ComponentUI.h"

class Collider3DUI : public ComponentUI
{
private:
	int m_iSocketIdx;

private:
	virtual void render_update() override;

public:
	Collider3DUI();
	virtual ~Collider3DUI();
};
