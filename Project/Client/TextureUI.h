#pragma once
#include "ResInfoUI.h"

class TextureUI : public ResInfoUI
{
private:
	float m_fAlphaSize = 50.f;

public:
	virtual void update() override;
	virtual void render_update() override;


public:
	TextureUI();
	virtual ~TextureUI();
};
