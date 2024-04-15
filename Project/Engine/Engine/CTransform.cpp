#include "pch.h"
#include "CTransform.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CSerializer.h"

CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
	, m_vRelativeScale(Vec3(1.f, 1.f, 1.f))
	, m_arrRelativeDir{}
	, m_arrWorldDir{}
	, m_bIgnoreParentScale(false)
	, m_bIgnoreParent{false} {}

CTransform::~CTransform() = default;

void CTransform::finalupdate()
{
	m_vRelativePos += m_vecMovePosition;
	// 크기 x 회전(자전) x 이동
	Matrix matScale       = XMMatrixScaling(m_vRelativeScale.x, m_vRelativeScale.y, m_vRelativeScale.z);
	Matrix matTranslation = XMMatrixTranslation(m_vRelativePos.x, m_vRelativePos.y, m_vRelativePos.z);

	//오일러 방식이므로, 쿼터니언으로 해야 짐벌락 방지 가능
	Matrix matRotX        = XMMatrixRotationX(m_vRelativeRot.x);
	Matrix matRotY        = XMMatrixRotationY(m_vRelativeRot.y);
	Matrix matRotZ        = XMMatrixRotationZ(m_vRelativeRot.z);
	m_matRelativeRot      = matRotX * matRotY * matRotZ;
	//m_matRelativeRot = XMMatrixRotationRollPitchYaw(m_vRelativeRot.x, m_vRelativeRot.y, m_vRelativeRot.z);

	// World 행렬 만들기
	m_matWorld = matScale * m_matRelativeRot * matTranslation;

	// RelativeDir 구하기
	static Vec3 vAxis[static_cast<UINT>(DIR_TYPE::END)] = {Vec3::Right, Vec3::Up, Vec3::Front};

	for (size_t i = 0; i < std::size(m_arrWorldDir); ++i)
	{
		//vAxis[i](회전하지 않았을 때의 기저축) x 회전행렬
		m_arrWorldDir[i] = m_arrRelativeDir[i] = XMVector3TransformNormal(vAxis[i], m_matRelativeRot);
	}

	if (GetOwner()->GetParent() && false == m_bIgnoreParent)
	{
		Matrix matParentWorld = GetOwner()->GetParent()->Transform()->GetWorldMat();

		if (m_bIgnoreParentScale)
		{
			Vec3   vParentWorldScale = GetOwner()->GetParent()->Transform()->GetWorldScale();
			Matrix matParentScale    = XMMatrixScaling(vParentWorldScale.x, vParentWorldScale.y, vParentWorldScale.z);
			Matrix matParentScaleInv = XMMatrixInverse(nullptr, matParentScale);

			m_matWorld = m_matWorld * matParentScaleInv * matParentWorld;
		}
		else
		{
			m_matWorld *= matParentWorld;
		}

		// World Dir 구하기
		for (size_t i = 0; i < std::size(m_arrWorldDir); ++i)
		{
			m_arrWorldDir[i] = XMVector3TransformNormal(m_arrRelativeDir[i], matParentWorld);
			m_arrWorldDir[i].Normalize();
		}
	}

	// 월드 역행렬
	m_matWorldInv = XMMatrixInverse(nullptr, m_matWorld);

	// m_vecMovePosition 초기화
	m_vecMovePosition = Vec3(0.f, 0.f, 0.f);
}

void CTransform::finalupdate_module()
{
	finalupdate();
}

void CTransform::active()
{
	CComponent::active();
	LOG_ASSERT(IsActive(), "TRANSFORM SHOULD NOT BE INACTIVE");
}

void CTransform::deactive()
{
	CComponent::deactive();
	LOG_ASSERT(IsActive(), "TRANSFORM SHOULD NOT BE INACTIVE");
}


Vec3 CTransform::GetWorldScale()
{
	Vec3 vWorldScale = m_vRelativeScale;

	CGameObject* pParent = GetOwner()->GetParent();
	if (m_bIgnoreParentScale)
	{
		pParent = nullptr;
	}

	while (pParent)
	{
		vWorldScale *= pParent->Transform()->GetRelativeScale();

		const bool bIgnoreParentScale = pParent->Transform()->m_bIgnoreParentScale;
		pParent                       = pParent->GetParent();

		if (bIgnoreParentScale)
		{
			pParent = nullptr;
		}
	}

	return vWorldScale;
}

Matrix CTransform::GetWorldRotation() const
{
	Matrix matWorldRot = m_matRelativeRot;

	CGameObject* pParent = GetOwner()->GetParent();
	while (pParent)
	{
		matWorldRot *= pParent->Transform()->m_matRelativeRot;
		pParent = pParent->GetParent();
	}

	return matWorldRot;
}

void CTransform::UpdateData()
{
	g_transform.matWorld    = m_matWorld;
	g_transform.matWorldInv = m_matWorldInv;
	g_transform.matWV       = g_transform.matWorld * g_transform.matView;
	g_transform.matWVP      = g_transform.matWV * g_transform.matProj;

	CConstBuffer* pBuffer = CDevice::GetInst()->GetCB(CB_TYPE::TRANSFORM);
	pBuffer->SetData(&g_transform, sizeof(tTransform));
	pBuffer->UpdateData();
}

void CTransform::SaveToScene(FILE* _pFile)
{
	CComponent::SaveToScene(_pFile);

	fwrite(&m_vRelativePos, sizeof(Vec3), 1, _pFile);
	fwrite(&m_vRelativeScale, sizeof(Vec3), 1, _pFile);
	fwrite(&m_vRelativeRot, sizeof(Vec3), 1, _pFile);
	fwrite(&m_bIgnoreParentScale, sizeof(bool), 1, _pFile);
}

void CTransform::LoadFromScene(FILE* _pFile)
{
	CComponent::LoadFromScene(_pFile);

	fread(&m_vRelativePos, sizeof(Vec3), 1, _pFile);
	fread(&m_vRelativeScale, sizeof(Vec3), 1, _pFile);
	fread(&m_vRelativeRot, sizeof(Vec3), 1, _pFile);
	fread(&m_bIgnoreParentScale, sizeof(bool), 1, _pFile);
}

void CTransform::Serialize(YAML::Emitter& emitter)
{
	emitter << YAML::Key << NAME_OF(m_vRelativePos) << YAML::Value << m_vRelativePos;
	emitter << YAML::Key << NAME_OF(m_vRelativeScale) << YAML::Value << m_vRelativeScale;
	emitter << YAML::Key << NAME_OF(m_vRelativeRot) << YAML::Value << m_vRelativeRot;
	emitter << YAML::Key << NAME_OF(m_bIgnoreParentScale) << YAML::Value << m_bIgnoreParentScale;
}

void CTransform::Deserialize(const YAML::Node& node)
{
	m_vRelativePos       = node[NAME_OF(m_vRelativePos)].as<Vec3>();
	m_vRelativeScale     = node[NAME_OF(m_vRelativeScale)].as<Vec3>();
	m_vRelativeRot       = node[NAME_OF(m_vRelativeRot)].as<Vec3>();
	m_bIgnoreParentScale = node[NAME_OF(m_bIgnoreParentScale)].as<bool>();
}
