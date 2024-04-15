#include "pch.h"
#include "CCollider3D.h"

#include "CConstBuffer.h"
#include "CDevice.h"
#include "CScript.h"
#include "CTransform.h"
#include "CSerializer.h"


CCollider3D::CCollider3D()
	: CComponent{ COMPONENT_TYPE::COLLIDER3D }
	, m_vOffsetScale{ 1.f, 1.f, 1.f }
	, m_iCollisionCount{ 0 }
	, m_eCollider3DType{ COLLIDER3D_TYPE::CUBE }
	, m_fLifeTime(-1.f)
{
	// Debug Obj 추가
	if (nullptr == m_pDebugObj)
	{
		m_pDebugObj = new CGameObject;
		m_pDebugObj->SetName(L"Collider_debug");
		m_pDebugObj->AddComponent(new CTransform);
		m_pDebugObj->AddComponent(new CMeshRender);

		m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh");
		m_pMaterial = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\Collider3D.mtrl");
	}
}

CCollider3D::CCollider3D(const CCollider3D& _origin)
	: CComponent{ COMPONENT_TYPE::COLLIDER3D }
	, m_vOffsetPos{ _origin.m_vOffsetPos }
	, m_vOffsetScale{ _origin.m_vOffsetScale }
	, m_iCollisionCount{ 0 }
	, m_eCollider3DType{ _origin.m_eCollider3DType }
	, m_pMesh{ _origin.m_pMesh }
	, m_pMaterial{ _origin.m_pMaterial }
	, m_fLifeTime(-1.f)
{
	// Debug Obj 추가
	if (nullptr == m_pDebugObj)
	{
		m_pDebugObj = new CGameObject;
		m_pDebugObj->SetName(L"Collider_debug");
		m_pDebugObj->AddComponent(new CTransform);
		m_pDebugObj->AddComponent(new CMeshRender);
	}
}

CCollider3D::~CCollider3D() {}

void CCollider3D::SetCollider3DType(COLLIDER3D_TYPE _type)
{
	if (COLLIDER3D_TYPE::CUBE == _type)
	{
		m_eCollider3DType = _type;
		m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh");
	}
	else
	{
		m_eCollider3DType = _type;
		m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh");
	}
}

void CCollider3D::SetOffsetPos(Vec3 _vOffsetPos)
{
	m_vOffsetPos = _vOffsetPos;
}

void CCollider3D::SetOffsetScale(Vec3 _vOffsetScale)
{
	m_vOffsetScale = _vOffsetScale;

	if (COLLIDER3D_TYPE::SPHERE == m_eCollider3DType)
	{
		m_vOffsetScale.x = m_vOffsetScale.y = m_vOffsetScale.z;
	}
}

COLLIDER3D_TYPE CCollider3D::GetCollider3DType() const
{
	return m_eCollider3DType;
}

Vec3 CCollider3D::GetWorldPos() const
{
	return m_matWorld.Translation();
}

Vec3 CCollider3D::GetWorldScale() const
{
	return m_vOffsetScale;
}

Matrix CCollider3D::GetWorldMatrix() const
{
	return m_matWorld;
}

Vec3 CCollider3D::GetOffsetPos() const
{
	return m_vOffsetPos;
}

Vec3 CCollider3D::GetOffsetScale() const
{
	return m_vOffsetScale;
}

void CCollider3D::OnCollisionEnter(CCollider3D* _pOther)
{
	++m_iCollisionCount;

	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (auto& pScript : vecScript)
	{
		pScript->OnCollisionEnter(_pOther->GetOwner());
	}
}

void CCollider3D::OnCollision(CCollider3D* _pOther)
{
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (auto& pScript : vecScript)
	{
		pScript->OnCollision(_pOther->GetOwner());
	}
}

void CCollider3D::OnCollisionExit(CCollider3D* _pOther)
{
	--m_iCollisionCount;
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (auto& pScript : vecScript)
	{
		pScript->OnCollisionExit(_pOther->GetOwner());
	}
}


void CCollider3D::UpdateData()
{
	static CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::TRANSFORM);
	g_transform.matWorld = m_matWorld;
	g_transform.matWorldInv = XMMatrixInverse(nullptr, m_matWorld);
	g_transform.matWV = g_transform.matWorld * g_transform.matView;
	g_transform.matWVP = g_transform.matWV * g_transform.matProj;

	pCB->SetData(&g_transform, sizeof(tTransform));
	pCB->UpdateData();
}

void CCollider3D::finalupdate()
{
	if (-1.f != m_fLifeTime)
	{
		m_fLifeTime -= DT;
		if (m_fLifeTime <= 0.f)
		{
			GetOwner()->Destroy();
			return;
		}
	}

	const Matrix matTranslation = XMMatrixTranslation(m_vOffsetPos.x, m_vOffsetPos.y, m_vOffsetPos.z);
	const Matrix matRotation = Matrix{};
	const Matrix matScale = XMMatrixScaling(m_vOffsetScale.x, m_vOffsetScale.y, m_vOffsetScale.z);
	m_matWorld = matScale * matRotation * matTranslation;

	const Vec3   vGameObjectScale = Transform()->GetWorldScale();
	const Matrix matGameObjectScaleInverse = XMMatrixInverse(nullptr,
		XMMatrixScaling(vGameObjectScale.x,
			vGameObjectScale.y,
			vGameObjectScale.z));
	const Matrix matGameobjectWorld = Transform()->GetWorldMat();
	// 충돌체 상대행렬 * 오브젝트 월드 크기 역행렬 * 오브젝트 월드 행렬(크기 * 회전 * 이동)
	m_matWorld = m_matWorld * matGameObjectScaleInverse * matGameobjectWorld;
}

void CCollider3D::finalupdate_debug()
{
	if (nullptr != m_pDebugObj)
	{
		m_pDebugObj->Transform()->SetRelativePos(Transform()->GetWorldPos() + m_vOffsetPos);
		m_pDebugObj->Transform()->SetRelativeScale(m_vOffsetScale);
		m_pDebugObj->Transform()->SetRelativeRotation(DecomposeRotMat(Transform()->GetWorldRotation()));

		m_pDebugObj->MeshRender()->SetMesh(m_pMesh);
		m_pDebugObj->MeshRender()->SetSharedMaterial(m_pMaterial, 0);

		if (nullptr != m_pDebugObj->MeshRender()->GetDynamicMaterial(0))
			m_pDebugObj->MeshRender()->GetDynamicMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_0, &m_iCollisionCount);

		m_pDebugObj->finalupdate();
	}
}

void CCollider3D::render_debug()
{
	if (nullptr != m_pDebugObj)
	{
		m_pDebugObj->MeshRender()->render();
	}
}

void CCollider3D::render()
{
	UpdateData();
}

void CCollider3D::SaveToScene(FILE* _pFile)
{
	CComponent::SaveToScene(_pFile);
	fwrite(&m_vOffsetPos, sizeof(Vec3), 1, _pFile);
	fwrite(&m_vOffsetScale, sizeof(Vec3), 1, _pFile);
	fwrite(&m_eCollider3DType, sizeof(UINT), 1, _pFile);
}

void CCollider3D::LoadFromScene(FILE* _pFile)
{
	fread(&m_vOffsetPos, sizeof(Vec2), 1, _pFile);
	fread(&m_vOffsetScale, sizeof(Vec2), 1, _pFile);
	fread(&m_eCollider3DType, sizeof(UINT), 1, _pFile);

	SetCollider3DType(m_eCollider3DType);
}

void CCollider3D::Serialize(YAML::Emitter& emitter)
{
	emitter << YAML::Key << NAME_OF(m_eCollider3DType) << YAML::Value << (int)m_eCollider3DType;
	emitter << YAML::Key << NAME_OF(m_vOffsetPos) << YAML::Value << m_vOffsetPos;
	emitter << YAML::Key << NAME_OF(m_vOffsetScale) << YAML::Value << m_vOffsetScale;
}

void CCollider3D::Deserialize(const YAML::Node& node)
{
	m_eCollider3DType = (COLLIDER3D_TYPE)node[NAME_OF(m_eCollider3DType)].as<int>();
	m_vOffsetPos = node[NAME_OF(m_vOffsetPos)].as<Vec3>();
	m_vOffsetScale = node[NAME_OF(m_vOffsetScale)].as<Vec3>();

	SetCollider3DType(m_eCollider3DType);
}
