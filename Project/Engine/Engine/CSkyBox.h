#pragma once
#include "CRenderComponent.h"

enum class SKYBOX_TYPE
{
	SPHERE,
	CUBE,
};

class CSkyBox : public CRenderComponent
{
private:
	SKYBOX_TYPE m_eType;

public:
	void SetSkyboxType(SKYBOX_TYPE _eType);

public:
	void finalupdate() override;
	void UpdateData() override;
	void render() override;

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(CSkyBox);
	CSkyBox();
	virtual ~CSkyBox();
};
