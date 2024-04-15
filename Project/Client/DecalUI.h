#pragma once
#include "ComponentUI.h"

class DecalUI final
	: public ComponentUI
{
private:
	CDecal* m_pTargetDecal;

private:
	void TextureSelect(void* _pTextureName);

public:
	void render_update() override;

public:
	DecalUI();
	virtual ~DecalUI();
};
