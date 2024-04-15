#include "pch.h"
#include "CLight2D.h"

#include "CRenderMgr.h"
#include "CTransform.h"
#include "CSerializer.h"

CLight2D::CLight2D()
	: CComponent(COMPONENT_TYPE::LIGHT2D)
	, m_LightInfo{}
	, m_iLightIdx(-1) {}

CLight2D::~CLight2D() = default;

void CLight2D::finalupdate()
{
	m_LightInfo.vWorldPos = Transform()->GetWorldPos();
	m_LightInfo.vLightDir = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	m_iLightIdx           = CRenderMgr::GetInst()->RegisterLight2D(this);
}

void CLight2D::SaveToScene(FILE* _pFile)
{
	CComponent::SaveToScene(_pFile);
	fwrite(&m_LightInfo, sizeof(tLightInfo), 1, _pFile);
}

void CLight2D::LoadFromScene(FILE* _pFile)
{
	CComponent::LoadFromScene(_pFile);
	fread(&m_LightInfo, sizeof(tLightInfo), 1, _pFile);
}

void CLight2D::Serialize(YAML::Emitter& emitter)
{
	emitter << YAML::Key << "LIGHT INFO" << YAML::Value << YAML::Flow << YAML::BeginMap;
	emitter << YAML::Key << NAME_OF(m_LightInfo.color.vDiff) << YAML::Value << m_LightInfo.color.vDiff;
	emitter << YAML::Key << NAME_OF(m_LightInfo.color.vSpec) << YAML::Value << m_LightInfo.color.vSpec;
	emitter << YAML::Key << NAME_OF(m_LightInfo.color.vAmb) << YAML::Value << m_LightInfo.color.vAmb;
	emitter << YAML::Key << NAME_OF(m_LightInfo.vLightDir) << YAML::Value << m_LightInfo.vLightDir;
	emitter << YAML::Key << NAME_OF(m_LightInfo.iLightType) << YAML::Value << m_LightInfo.iLightType;
	emitter << YAML::Key << NAME_OF(m_LightInfo.vWorldPos) << YAML::Value << m_LightInfo.vWorldPos;
	emitter << YAML::Key << NAME_OF(m_LightInfo.fAngle) << YAML::Value << m_LightInfo.fAngle;
	emitter << YAML::Key << NAME_OF(m_LightInfo.fRange) << YAML::Value << m_LightInfo.fRange;
	emitter << YAML::EndMap;
}

void CLight2D::Deserialize(const YAML::Node& node)
{
	YAML::Node lightInfoNode = node["LIGHT INFO"];

	m_LightInfo.color.vDiff = lightInfoNode[NAME_OF(m_LightInfo.color.vDiff)].as<Vec4>();
	m_LightInfo.color.vSpec = lightInfoNode[NAME_OF(m_LightInfo.color.vSpec)].as<Vec4>();
	m_LightInfo.color.vAmb  = lightInfoNode[NAME_OF(m_LightInfo.color.vAmb)].as<Vec4>();
	m_LightInfo.vLightDir   = lightInfoNode[NAME_OF(m_LightInfo.vLightDir)].as<Vec3>();
	m_LightInfo.iLightType  = lightInfoNode[NAME_OF(m_LightInfo.iLightType)].as<int>();
	m_LightInfo.vWorldPos   = lightInfoNode[NAME_OF(m_LightInfo.vWorldPos)].as<Vec3>();
	m_LightInfo.fAngle      = lightInfoNode[NAME_OF(m_LightInfo.fAngle)].as<float>();
	m_LightInfo.fRange      = lightInfoNode[NAME_OF(m_LightInfo.fRange)].as<float>();

	SetLightType(static_cast<LIGHT_TYPE>(m_LightInfo.iLightType));
}
