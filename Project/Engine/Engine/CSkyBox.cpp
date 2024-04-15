#include "pch.h"
#include "CSkyBox.h"

#include "CTransform.h"
#include "CSerializer.h"

CSkyBox::CSkyBox()
	: CRenderComponent(COMPONENT_TYPE::SKYBOX)
	, m_eType(SKYBOX_TYPE::SPHERE)
{
	SetSkyboxType(m_eType);
}

CSkyBox::~CSkyBox() = default;

void CSkyBox::finalupdate() {}

void CSkyBox::UpdateData()
{
	GetMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_0, (int*)&m_eType);
}

void CSkyBox::render()
{
	if (nullptr == GetMesh() || nullptr == GetMaterial(0))
	{
		return;
	}
	UpdateData();

	Transform()->UpdateData();
	GetMaterial(0)->UpdateData();
	GetMesh()->render(0);
}

void CSkyBox::SetSkyboxType(SKYBOX_TYPE _eType)
{
	m_eType = _eType;
	if (SKYBOX_TYPE::SPHERE == m_eType)
	{
		SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	}
	else
	{
		SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	}
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\SkyBoxMtrl.mtrl"), 0);
}

void CSkyBox::SaveToScene(FILE* _pFile)
{
	CRenderComponent::SaveToScene(_pFile);
	// SkyBox Type
	fwrite(&m_eType, sizeof(UINT), 1, _pFile);
}

void CSkyBox::LoadFromScene(FILE* _pFile)
{
	CRenderComponent::LoadFromScene(_pFile);
	// SkyBox Type
	fread(&m_eType, sizeof(UINT), 1, _pFile);
	SetSkyboxType(m_eType);
}

void CSkyBox::Serialize(YAML::Emitter& emitter)
{
	CRenderComponent::Serialize(emitter);
	emitter << YAML::Key << NAME_OF(m_eType) << YAML::Value << static_cast<int>(m_eType);
}

void CSkyBox::Deserialize(const YAML::Node& node)
{
	CRenderComponent::Deserialize(node);
	m_eType = static_cast<SKYBOX_TYPE>(node[NAME_OF(m_eType)].as<int>());
	SetSkyboxType(m_eType);
}
