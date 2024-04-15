#include "pch.h"
#include "CBoundingBox.h"
#include "CTransform.h"
#include "CDevice.h"
#include "CSerializer.h"
#include "CMeshRender.h"
#include "CConstBuffer.h"

CBoundingBox::CBoundingBox()
	: CComponent{COMPONENT_TYPE::BOUNDINGBOX}
	, m_vColor(1.f, 1.f, 1.f, 1.f)
	, m_vOffsetPos{0.f, 0.f, 0.f}
	, m_vOffsetScale{100.f, 100.f, 100.f}
	, m_bBoundCheck(false)
{
	// Debug Obj 추가
	if (nullptr == m_pDebugObj)
	{
		m_pDebugObj = new CGameObject;
		m_pDebugObj->SetName(L"BoundingBox_debug");
		m_pDebugObj->AddComponent(new CTransform);
		m_pDebugObj->AddComponent(new CMeshRender);

		m_pMesh     = CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh");
		m_pMaterial = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\Std3DWireShader.mtrl");
	}
}

CBoundingBox::CBoundingBox(const CBoundingBox& _origin)
	: CComponent{COMPONENT_TYPE::BOUNDINGBOX}
	, m_vOffsetPos(_origin.m_vOffsetPos)
	, m_vOffsetScale(_origin.m_vOffsetScale)
	, m_bBoundCheck(_origin.m_bBoundCheck) {}

CBoundingBox::~CBoundingBox() = default;

void CBoundingBox::UpdateData()
{
	static CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::TRANSFORM);
	g_transform.matWorld     = m_matWorld;
	g_transform.matWorldInv  = XMMatrixInverse(nullptr, m_matWorld);
	g_transform.matWV        = g_transform.matWorld * g_transform.matView;
	g_transform.matWVP       = g_transform.matWV * g_transform.matProj;

	pCB->SetData(&g_transform, sizeof(tTransform));
	pCB->UpdateData();
}

void CBoundingBox::finalupdate()
{
	const Matrix matTranslation = XMMatrixTranslation(m_vOffsetPos.x, m_vOffsetPos.y, m_vOffsetPos.z);
	const Matrix matRotation    = Matrix{};
	const Matrix matScale       = XMMatrixScaling(m_vOffsetScale.x, m_vOffsetScale.y, m_vOffsetScale.z);
	m_matWorld                  = matScale * matRotation * matTranslation;

	const Vec3   vGameObjectScale          = Transform()->GetWorldScale();
	const Matrix matGameObjectScaleInverse = XMMatrixInverse(nullptr,
	                                                         XMMatrixScaling(vGameObjectScale.x,
	                                                                         vGameObjectScale.y,
	                                                                         vGameObjectScale.z));
	const Matrix matGameobjectWorld = Transform()->GetWorldMat();
	// 충돌체 상대행렬 * 오브젝트 월드 크기 역행렬 * 오브젝트 월드 행렬(크기 * 회전 * 이동)
	m_matWorld = m_matWorld * matGameObjectScaleInverse * matGameobjectWorld;
}


void CBoundingBox::finalupdate_debug()
{
	if (nullptr != m_pDebugObj)
	{
		m_pDebugObj->Transform()->SetRelativePos(Transform()->GetWorldPos() + m_vOffsetPos);
		m_pDebugObj->Transform()->SetRelativeScale(m_vOffsetScale);
		m_pDebugObj->Transform()->SetRelativeRotation(DecomposeRotMat(Transform()->GetWorldRotation()));

		m_pDebugObj->MeshRender()->SetMesh(m_pMesh);
		m_pDebugObj->MeshRender()->SetSharedMaterial(m_pMaterial, 0);
		if (nullptr != m_pDebugObj->MeshRender()->GetDynamicMaterial(0))
			m_pDebugObj->MeshRender()->GetDynamicMaterial(0)->SetScalarParam(SCALAR_PARAM::VEC4_0, &m_vColor);

		m_pDebugObj->finalupdate();
	}
}

void CBoundingBox::render()
{
	UpdateData();
}

void CBoundingBox::render_debug()
{
	if (nullptr == m_pDebugObj)
		return;

	if (!m_pDebugObj->IsActive())
		return;

	m_pDebugObj->Transform()->UpdateData();

	m_pDebugObj->MeshRender()->GetMaterial(0)->UpdateData();
	m_pDebugObj->MeshRender()->GetMesh()->render(0);
}

void CBoundingBox::SetOffsetPos(Vec3 _vOffsetPos)
{
	m_vOffsetPos = _vOffsetPos;
}

void CBoundingBox::SetOffsetScale(Vec3 _vOffsetScale)
{
	m_vOffsetScale = _vOffsetScale;
}

void CBoundingBox::SetColor(Vec4 vColor)
{
	m_vColor = vColor;
}


Vec3 CBoundingBox::GetWorldPos() const
{
	return m_matWorld.Translation();
}

Vec3 CBoundingBox::GetWorldScale() const
{
	return m_vOffsetScale;
}

Matrix CBoundingBox::GetWorldMatrix() const
{
	return m_matWorld;
}

Vec3 CBoundingBox::GetOffsetPos() const
{
	return m_vOffsetPos;
}

Vec3 CBoundingBox::GetOffsetScale() const
{
	return m_vOffsetScale;
}

void CBoundingBox::Serialize(YAML::Emitter& emitter)
{
	emitter << YAML::Key << NAME_OF(m_vOffsetPos) << YAML::Value << m_vOffsetPos;
	emitter << YAML::Key << NAME_OF(m_vOffsetScale) << YAML::Value << m_vOffsetScale;
}

void CBoundingBox::Deserialize(const YAML::Node& node)
{
	m_vOffsetPos   = node[NAME_OF(m_vOffsetPos)].as<Vec3>();
	m_vOffsetScale = node[NAME_OF(m_vOffsetScale)].as<Vec3>();
}

void CBoundingBox::SaveToScene(FILE* _pFile)
{
	CComponent::SaveToScene(_pFile);
	fwrite(&m_vOffsetPos, sizeof(Vec3), 1, _pFile);
	fwrite(&m_vOffsetScale, sizeof(Vec3), 1, _pFile);
}

void CBoundingBox::LoadFromScene(FILE* _pFile)
{
	fread(&m_vOffsetPos, sizeof(Vec2), 1, _pFile);
	fread(&m_vOffsetScale, sizeof(Vec2), 1, _pFile);
}
