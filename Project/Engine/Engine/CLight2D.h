#pragma once
#include "CComponent.h"

class CLight2D : public CComponent
{
private:
	tLightInfo m_LightInfo;
	int        m_iLightIdx;

public:
	const tLightInfo& GetLightInfo() const { return m_LightInfo; }
	LIGHT_TYPE        GetLightType() const { return static_cast<LIGHT_TYPE>(m_LightInfo.iLightType); }

	void SetDiffuse(Vec3 _vDiffuse) { m_LightInfo.color.vDiff = _vDiffuse; }
	void SetSpecular(Vec3 _vSpec) { m_LightInfo.color.vSpec = _vSpec; }
	void SetAmbient(Vec3 _vAmb) { m_LightInfo.color.vAmb = _vAmb; }
	void SetLightDir(Vec3 _vDir) { m_LightInfo.vLightDir = _vDir; }
	void SetRange(float _fRange) { m_LightInfo.fRange = _fRange; }
	void SetAngle(float _fAngle) { m_LightInfo.fAngle = _fAngle; }
	void SetLightType(LIGHT_TYPE _eType) { m_LightInfo.iLightType = static_cast<int>(_eType); }

public:
	void finalupdate() override;

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(CLight2D);
	CLight2D();
	virtual ~CLight2D();
};
