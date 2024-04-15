#include "pch.h"
#include "CLight3D.h"

#include "CRenderMgr.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CScene.h"
#include "CCamera.h"
#include "CSerializer.h"

CLight3D::CLight3D()
	: CComponent(COMPONENT_TYPE::LIGHT3D)
	, m_LightInfo{}
	, m_iLightIdx(-1)
	, m_pLightCam(nullptr)
{
	m_pLightCam = new CGameObject;
	m_pLightCam->AddComponent(new CTransform);
	m_pLightCam->AddComponent(new CCamera);

	// Debug Obj 추가
	m_pDebugObj = new CGameObject;
	m_pDebugObj->SetName(L"Light_debug");
	m_pDebugObj->AddComponent(new CTransform);
	m_pDebugObj->AddComponent(new CMeshRender);
	m_pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	m_pDebugObj->MeshRender()->
	             SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\Std3DWireShader.mtrl"), 0);
}

CLight3D::CLight3D(const CLight3D& _origin)
	: CComponent(_origin)
	, m_LightInfo(_origin.m_LightInfo)
	, m_iLightIdx(-1)
	, m_pLightCam(nullptr)
{
	m_pLightCam = _origin.m_pLightCam->Clone();
}

CLight3D::~CLight3D()
{
	SAFE_DELETE(m_pLightCam);
}

void CLight3D::SetLightType(LIGHT_TYPE _eType)
{
	m_LightInfo.iLightType = (int)_eType;

	switch (_eType)
	{
	case LIGHT_TYPE::DIRECTIONAL:
		m_pVolumeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
		m_pLightMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\DirLightMtrl.mtrl");
		m_pLightCam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
		m_pLightCam->Camera()->SetWidth(4096.f);
		m_pLightCam->Camera()->SetAspectRatio(1.f);
		m_pLightCam->Camera()->SetFar(100000.f);
		break;
	case LIGHT_TYPE::POINT:
		m_pVolumeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh");
		m_pLightMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\PointLightMtrl.mtrl");
		break;
	case LIGHT_TYPE::SPOT:
		m_pVolumeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"ConeMesh");
		m_pLightMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\SpotLightMtrl.mtrl");
		break;
	}
}

void CLight3D::SetLightDir(Vec3 _vDir)
{
	m_LightInfo.vLightDir = _vDir;
	m_LightInfo.vLightDir.Normalize();

	Vec3 vFront = m_LightInfo.vLightDir;
	Vec3 vUp    = Vec3(0.f, 1.f, 0.f);
	Vec3 vRight = XMVector3Cross(vUp, vFront);
	vRight.Normalize();
	vUp = XMVector3Cross(vFront, vRight);
	vUp.Normalize();

	Matrix matRot = XMMatrixIdentity();
	matRot._11    = vRight.x;
	matRot._12    = vRight.y;
	matRot._13    = vRight.z;
	matRot._21    = vUp.x;
	matRot._22    = vUp.y;
	matRot._23    = vUp.z;
	matRot._31    = vFront.x;
	matRot._32    = vFront.y;
	matRot._33    = vFront.z;

	Vec3 vRot = DecomposeRotMat(matRot);

	// 광원이 가리키는 방향이 Transform 의 Front 가 되도록 회전시켜준다.
	Transform()->SetRelativeRotation(vRot);
}

void CLight3D::SetRange(float _fRange)
{
	m_LightInfo.fRange = _fRange;
	Transform()->SetRelativeScale(_fRange * 2.f, _fRange * 2.f, _fRange * 2.f);
}

void CLight3D::update() {}

void CLight3D::finalupdate()
{
	m_LightInfo.vWorldPos = Transform()->GetWorldPos();
	m_LightInfo.vLightDir = Transform()->GetWorldDir(DIR_TYPE::FRONT);
	m_LightInfo.fRange    = Transform()->GetWorldScale().x / 2.f;

	m_iLightIdx = CRenderMgr::GetInst()->RegisterLight3D(this);

	// 광원의 카메라도 광원과 동일한 Transform 이 되도록 업데이트 한다.
	m_pLightCam->Transform()->SetRelativePos(Transform()->GetWorldPos());
	m_pLightCam->Transform()->SetRelativeRotation(DecomposeRotMat(Transform()->GetWorldRotation()));
	m_pLightCam->finalupdate_module();
}

void CLight3D::finalupdate_debug()
{
	if (nullptr != m_pDebugObj)
	{
		m_pDebugObj->Transform()->SetRelativePos(Transform()->GetWorldPos());
		m_pDebugObj->Transform()->SetRelativeScale(Transform()->GetWorldScale());
		m_pDebugObj->Transform()->SetRelativeRotation(DecomposeRotMat(Transform()->GetWorldRotation()));

		m_pDebugObj->MeshRender()->SetMesh(m_pVolumeMesh);
		m_pDebugObj->MeshRender()->
		             SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\Std3DWireShader.mtrl"), 0);

		m_pDebugObj->finalupdate();
	}
}

void CLight3D::render()
{
	m_pLightMtrl->SetScalarParam(SCALAR_PARAM::INT_0, &m_iLightIdx);

	// 방향성 광원인 경우 그림자 처리를 위해서 광원카메라로 투영시킬 수 있게 View * Proj 행렬을 전달
	if (LIGHT_TYPE::DIRECTIONAL == (LIGHT_TYPE)m_LightInfo.iLightType)
	{
		Matrix matLightVP = m_pLightCam->Camera()->GetViewMat() * m_pLightCam->Camera()->GetProjMat();
		m_pLightMtrl->SetScalarParam(SCALAR_PARAM::MAT_0, &matLightVP);
	}

	Transform()->UpdateData();

	m_pLightMtrl->UpdateData();
	m_pVolumeMesh->render(0);
}

void CLight3D::render_shadowmap()
{
	m_pLightCam->Camera()->SortShadowObject();
	m_pLightCam->Camera()->render_shadowmap();
}

void CLight3D::render_debug()
{
	if (!m_pDebugObj->IsActive())
		return;

	if (LIGHT_TYPE::DIRECTIONAL == (LIGHT_TYPE)m_LightInfo.iLightType)
		return;

	m_pDebugObj->Transform()->UpdateData();

	if (nullptr != m_pDebugObj->MeshRender()->GetMesh()
	    && nullptr != m_pDebugObj->MeshRender()->GetMaterial(0))
	{
		m_pDebugObj->MeshRender()->GetMaterial(0)->UpdateData();
		m_pDebugObj->MeshRender()->GetMesh()->render(0);
	}
}

void CLight3D::SaveToScene(FILE* _pFile)
{
	CComponent::SaveToScene(_pFile);

	fwrite(&m_LightInfo, sizeof(tLightInfo), 1, _pFile);
}

void CLight3D::LoadFromScene(FILE* _pFile)
{
	CComponent::LoadFromScene(_pFile);

	fread(&m_LightInfo, sizeof(tLightInfo), 1, _pFile);
}


void CLight3D::Serialize(YAML::Emitter& emitter)
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

void CLight3D::Deserialize(const YAML::Node& node)
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

	SetLightType((LIGHT_TYPE)m_LightInfo.iLightType);
}
