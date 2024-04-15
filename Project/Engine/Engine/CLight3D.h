#pragma once
#include "CComponent.h"

class CLight3D : public CComponent
{
private:
	tLightInfo m_LightInfo;
	int        m_iLightIdx;

	Ptr<CMesh>     m_pVolumeMesh;
	Ptr<CMaterial> m_pLightMtrl;

	CGameObject* m_pLightCam;

public:
	const tLightInfo& GetLightInfo() const { return m_LightInfo; }
	LIGHT_TYPE        GetLightType() const { return static_cast<LIGHT_TYPE>(m_LightInfo.iLightType); }

	void SetDiffuse(Vec3 _vDiffuse) { m_LightInfo.color.vDiff = _vDiffuse; }
	void SetSpecular(Vec3 _vSpec) { m_LightInfo.color.vSpec = _vSpec; }
	void SetAmbient(Vec3 _vAmb) { m_LightInfo.color.vAmb = _vAmb; }
	void SetLightDir(Vec3 _vDir);
	void SetRange(float _fRange);
	void SetAngle(float _fAngle) { m_LightInfo.fAngle = _fAngle; }
	void SetLightType(LIGHT_TYPE _eType);

public:
	void update() override;
	void finalupdate() override;
	void finalupdate_debug() override;

	void render();
	void render_shadowmap();
	void render_debug() override;

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	void Serialize(YAML::Emitter& emitter) override;;
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(CLight3D);
	CLight3D();
	CLight3D(const CLight3D& _origin);
	virtual ~CLight3D();
};
