#pragma once
#include "ResInfoUI.h"

class MaterialUI : public ResInfoUI
{
private:
	TEX_PARAM m_eSelectedTexParam;

public:
	virtual void update() override;
	virtual void render_update() override;

public:
	void TextureSelected(DWORD_PTR _ptr);

public:
	MaterialUI();
	virtual ~MaterialUI();
};
