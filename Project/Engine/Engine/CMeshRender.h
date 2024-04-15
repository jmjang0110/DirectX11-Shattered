#pragma once
#include "CRenderComponent.h"

class CMeshRender : public CRenderComponent
{
public:
	void finalupdate() override;
	void render() override;

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	CLONE(CMeshRender);
	CMeshRender();
	virtual ~CMeshRender();
};
