#include "pch.h"
#include "CDecal.h"

#include "CTransform.h"
#include "CMeshRender.h"
#include "CSerializer.h"

CDecal::CDecal()
	: CRenderComponent(COMPONENT_TYPE::DECAL)
	, m_eType(DECAL_TYPE::CUBE)
	, m_bUseDeferredLighting(false)
	, m_vColor(1.f, 1.f, 1.f, 1.f)
{
	SetDecalType(m_eType);

	// Debug Obj Ãß°¡
	if (nullptr == m_pDebugObj)
	{
		m_pDebugObj = new CGameObject;
		m_pDebugObj->SetName(L"Decal_debug");
		m_pDebugObj->AddComponent(new CTransform);
		m_pDebugObj->AddComponent(new CMeshRender);
	}
}

CDecal::~CDecal() = default;

void CDecal::SetDecalType(DECAL_TYPE _eType)
{
	m_eType = _eType;
	if (DECAL_TYPE::CUBE == m_eType)
	{
		SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
		SetDeferredLighting(m_bUseDeferredLighting);
	}
	else
	{
		SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
		SetDeferredLighting(m_bUseDeferredLighting);
	}
}

void CDecal::SetDeferredLighting(bool _bLighting)
{
	m_bUseDeferredLighting = _bLighting;

	Ptr<CMaterial> pMtrl = nullptr;
	if (m_bUseDeferredLighting)
	{
		pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\DeferredDecalMtrl.mtrl");
	}
	else
	{
		pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\ForwardDecalMtrl.mtrl");
	}

	SetSharedMaterial(pMtrl, 0);
}

std::string CDecal::GetDecalTypeName(DECAL_TYPE _type) const
{
	return magic_enum::enum_name(_type).data();
}

void CDecal::finalupdate() {}

void CDecal::finalupdate_debug()
{
	if (nullptr != m_pDebugObj)
	{
		CMaterial* pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\Std3DWireShader.mtrl").Get();

		m_pDebugObj->MeshRender()->SetMesh(GetMesh());
		m_pDebugObj->MeshRender()->SetSharedMaterial(pMtrl, 0);

		m_pDebugObj->Transform()->SetRelativePos(Transform()->GetWorldPos());
		m_pDebugObj->Transform()->SetRelativeScale(Transform()->GetWorldScale());
		m_pDebugObj->Transform()->SetRelativeRotation(DecomposeRotMat(Transform()->GetWorldRotation()));

		m_pDebugObj->finalupdate();
	}
}

void CDecal::UpdateData()
{
	GetMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_0, (int*)&m_eType);
	GetMaterial(0)->SetScalarParam(SCALAR_PARAM::VEC4_0, &m_vColor);
	//GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_1, m_pDecalTex);
}

void CDecal::render()
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

void CDecal::render_debug()
{
	if (!m_pDebugObj->IsActive())
	{
		return;
	}

	m_pDebugObj->Transform()->UpdateData();

	m_pDebugObj->MeshRender()->GetMaterial(0)->UpdateData();
	m_pDebugObj->MeshRender()->GetMesh()->render(0);
}

void CDecal::SaveToScene(FILE* _pFile)
{
	CRenderComponent::SaveToScene(_pFile);

	// SkyBox Type
	fwrite(&m_eType, sizeof(UINT), 1, _pFile);
}

void CDecal::LoadFromScene(FILE* _pFile)
{
	CRenderComponent::LoadFromScene(_pFile);

	// SkyBox Type
	fread(&m_eType, sizeof(UINT), 1, _pFile);
	SetDecalType(m_eType);
}

void CDecal::Serialize(YAML::Emitter& emitter)
{
	CRenderComponent::Serialize(emitter);
	emitter << YAML::Key << NAME_OF(m_eType) << YAML::Value << (int)m_eType;
}

void CDecal::Deserialize(const YAML::Node& node)
{
	CRenderComponent::Deserialize(node);
	m_eType = (DECAL_TYPE)node[NAME_OF(m_eType)].as<int>();
	SetDecalType(m_eType);
}
