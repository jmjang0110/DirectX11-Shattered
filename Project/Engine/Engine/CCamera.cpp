#include "pch.h"
#include "CCamera.h"

#include "CDevice.h"
#include "CRenderMgr.h"
#include "CEventMgr.h"
#include "CResMgr.h"
#include "CKeyMgr.h"

#include "CSceneMgr.h"
#include "CScene.h"
#include "CLayer.h"
#include "CGameObject.h"

#include "CTransform.h"
#include "CMeshRender.h"
#include "CBoundingBox.h"

#include "CMRT.h"
#include "CSerializer.h"

CCamera::CCamera()
	: CComponent(COMPONENT_TYPE::CAMERA)
	, m_ray{}
	, m_eProjType(PROJ_TYPE::PERSPECTIVE)
	, m_fWidth(0.f)
	, m_fAspectRatio(1.f)
	, m_fFOV(XM_PI / 4.f)
	, m_fFar(10000.f)
	, m_iLayerMask(0)
	, m_iCamIdx(-1)
{
	m_fWidth       = CDevice::GetInst()->GetRenderResolution().x;
	m_fAspectRatio = CDevice::GetInst()->GetRenderResolution().x / CDevice::GetInst()->GetRenderResolution().y;

	m_Frustum.m_pCam = this;
}

CCamera::CCamera(const CCamera& _origin)
	: CComponent(_origin)
	, m_Frustum(_origin.m_Frustum)
	, m_eProjType(_origin.m_eProjType)
	, m_fWidth(_origin.m_fWidth)
	, m_fAspectRatio(_origin.m_fAspectRatio)
	, m_fFOV(_origin.m_fFOV)
	, m_fFar(_origin.m_fFar)
	, m_iLayerMask(_origin.m_iLayerMask)
	, m_iCamIdx(-1)
{
	m_Frustum.m_pCam = this;
}

CCamera::~CCamera() = default;

void CCamera::finalupdate()
{
	finalupdate_module();

	m_Frustum.finalupdate();

	CRenderMgr::GetInst()->RegisterCamera(this);
}

void CCamera::finalupdate_module()
{
	// View 행렬 계산
	const Vec3 cameraPos = Transform()->GetRelativePos();
	// View 이동행렬
	const Matrix matViewTranslation = XMMatrixTranslation(-cameraPos.x, -cameraPos.y, -cameraPos.z);
	// View 회전행렬
	// Right, Up, Front 를 가져온다.
	Matrix matViewRot = XMMatrixIdentity();

	const Vec3 vRight = Transform()->GetWorldRightDir();
	const Vec3 vUp    = Transform()->GetWorldUpDir();
	const Vec3 vFront = Transform()->GetWorldFrontDir();

	matViewRot._11 = vRight.x, matViewRot._12 = vUp.x, matViewRot._13 = vFront.x;
	matViewRot._21 = vRight.y, matViewRot._22 = vUp.y, matViewRot._23 = vFront.y;
	matViewRot._31 = vRight.z, matViewRot._32 = vUp.z, matViewRot._33 = vFront.z;

	m_matView    = matViewTranslation * matViewRot;
	m_matViewInv = XMMatrixInverse(nullptr, m_matView);

	// 투영행렬 계산
	if (PROJ_TYPE::ORTHOGRAPHIC == m_eProjType)
	{
		const float fHeight  = m_fWidth / m_fAspectRatio;
		const float fEpsilon = m_fFar - 1.f; // Far - Near
		if (fEpsilon <= 0.00001f)      // XMMatrixOrthographicLH / assert(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));
		{
			m_fFar += 0.00002f;
		}
		m_matProj = XMMatrixOrthographicLH(m_fWidth, fHeight, 1.f, m_fFar);
	}
	else
	{
		const float fEpsilon = m_fFar - 1.f; // Far - Near
		if (fEpsilon <= 0.00001f)      // XMMatrixOrthographicLH / assert(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));
		{
			m_fFar += 0.00002f;
		}
		m_matProj = XMMatrixPerspectiveFovLH(m_fFOV, m_fAspectRatio, 1.f, m_fFar);
	}

	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);
	CalRay();
}


void CCamera::SortGameObject()
{
	m_vecDeferred.clear();
	m_vecDeferredDecal.clear();
	m_vecDeferredParticle.clear();
	m_vecForward.clear();
	m_vecMasked.clear();
	m_vecForwardDecal.clear();
	m_vecTranslucent.clear();
	m_vecPostProcess.clear();
	m_vecComponentDebug.clear();

	const CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	for (UINT layerIndex = 0; layerIndex < MAX_LAYER; ++layerIndex)
	{
		// 카메라가 찍을 대상 레이어가 아니면 continue
		if (false == m_iLayerMask[layerIndex])
		{
			continue;
		}

		CLayer* pLayer = pCurScene->GetLayer(layerIndex);
		for (auto& pGameObject : pLayer->GetObjects())
		{
			// Register Component Debug Object
			SortDebugGameObject(pGameObject);

			// Register Render Component
			CRenderComponent* pRenderCom = pGameObject->GetRenderComponent();
			if (nullptr == pRenderCom
			    || nullptr == pRenderCom->GetMesh()
			    || nullptr == pRenderCom->GetMaterial(0)
			    || nullptr == pRenderCom->GetMaterial(0)->GetShader())
			{
				continue;
			}

			// 오브젝트가 카메라 시야 밖에 있으면 제외
			// TODO:바운딩 박스로 전환
			if (pRenderCom->IsFrustumCulling() && FrustumCulling(pGameObject))
			{
				continue;
			}

			Ptr<CGraphicsShader> pShader = pRenderCom->GetMaterial(0)->GetShader();
			switch (pShader->GetShaderDomain())
			{
			case SHADER_DOMAIN::DOMAIN_DEFERRED:
				m_vecDeferred.push_back(pGameObject);
				break;
			case SHADER_DOMAIN::DOMAIN_DEFERRED_DECAL:
				m_vecDeferredDecal.push_back(pGameObject);
				break;
			case SHADER_DOMAIN::DOMAIN_EMISSIVE_PARTICLE:
				m_vecDeferredParticle.push_back(pGameObject);
				m_vecTranslucent.push_back(pGameObject);
				break;
			case SHADER_DOMAIN::DOMAIN_FORWARD:
				m_vecForward.push_back(pGameObject);
				break;
			case SHADER_DOMAIN::DOMAIN_MASKED:
				m_vecMasked.push_back(pGameObject);
				break;
			case SHADER_DOMAIN::DOMAIN_FORWARD_DECAL:
				m_vecForwardDecal.push_back(pGameObject);
				break;
			case SHADER_DOMAIN::DOMAIN_TRANSLUCENT:
				m_vecTranslucent.push_back(pGameObject);
				break;
			case SHADER_DOMAIN::DOMAIN_POSTPROCESS:
				m_vecPostProcess.push_back(pGameObject);
				break;
			}
		}
	}
}

void CCamera::SortShadowObject()
{
	m_vecDynamicShadow.clear();

	const CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pCurScene->GetLayer(i);
		for (auto& pGameObject : pLayer->GetObjects())
		{
			const CRenderComponent* pRenderCom = pGameObject->GetRenderComponent();
			if (pRenderCom && pRenderCom->IsDynamicShadow())
			{
				m_vecDynamicShadow.push_back(pGameObject);
			}
		}
	}
}

void CCamera::SortDebugGameObject(CGameObject* _pObj)
{
	for (int i = 0; i < static_cast<int>(COMPONENT_TYPE::END); ++i)
	{
		CComponent* pComponents = _pObj->GetComponent(static_cast<COMPONENT_TYPE>(i));
		if (nullptr != pComponents && nullptr != pComponents->GetDebugObj())
		{
			m_vecComponentDebug.push_back(pComponents);
		}
	}
}

void CCamera::render_deferred()
{
	for (const auto& pGameObject : m_vecDeferred)
	{
		if (pGameObject->IsActive())
		{
			pGameObject->render();
		}
	}
}

void CCamera::render_deferred_decal()
{
	for (const auto& pGameObject : m_vecDeferredDecal)
	{
		if (pGameObject->IsActive())
		{
			pGameObject->render();
		}
	}
}

void CCamera::render_particle()
{
	for (const auto& pGameObject : m_vecDeferredParticle)
	{
		if (pGameObject->IsActive())
		{
			pGameObject->render();
		}
	}
}

void CCamera::render_forward()
{
	for (const auto& pGameObject : m_vecForward)
	{
		if (pGameObject->IsActive())
		{
			pGameObject->render();
		}
	}
}

void CCamera::render_forward_decal()
{
	for (const auto& pGameObject : m_vecForwardDecal)
	{
		if (pGameObject->IsActive())
		{
			pGameObject->render();
		}
	}
}

void CCamera::render_masked()
{
	for (const auto& pGameObject : m_vecMasked)
	{
		if (pGameObject->IsActive())
		{
			pGameObject->render();
		}
	}
}

void CCamera::render_translucent()
{
	for (const auto& pGameObject : m_vecTranslucent)
	{
		if (pGameObject->IsActive())
		{
			pGameObject->render();
		}
	}
}

void CCamera::render_postprocess()
{
	for (const auto& pGameObject : m_vecPostProcess)
	{
		if (pGameObject->IsActive())
		{
			CRenderMgr::GetInst()->CopyTargetToPostProcess();
			pGameObject->render();
		}
	}
}

void CCamera::render_shadowmap()
{
	// 광원 카메라의 View, Proj 세팅
	g_transform.matView    = m_matView;
	g_transform.matViewInv = m_matViewInv;
	g_transform.matProj    = m_matProj;

	for (const auto& pGameObject : m_vecDynamicShadow)
	{
		if (pGameObject->IsActive())
		{
			pGameObject->GetRenderComponent()->render_shadowmap();
		}
	}
}

void CCamera::render_debug()
{
	// Camera Debug Object render
	if (m_Frustum.GetShowFrustum())
	{
		m_Frustum.render();
	}

	// Other Component Debug Object render
	for (const auto& pComponent : m_vecComponentDebug)
	{
		if (pComponent->IsActive())
		{
			pComponent->render_debug();
		}
	}
}

void CCamera::SetCameraAsMain()
{
	tEventInfo tEvent = {};
	tEvent.eType      = EVENT_TYPE::SET_CAMEAR_INDEX;
	tEvent.lParam     = (DWORD_PTR)this;
	tEvent.wParam     = 0;
	CEventMgr::GetInst()->AddEvent(tEvent);
}

void CCamera::SetCameraIndex(int _iIdx)
{
	if (m_iCamIdx == _iIdx)
		return;

	tEventInfo tEvent = {};
	tEvent.eType      = EVENT_TYPE::SET_CAMEAR_INDEX;
	tEvent.lParam     = (DWORD_PTR)this;
	tEvent.wParam     = _iIdx;
	CEventMgr::GetInst()->AddEvent(tEvent);
}

void CCamera::CheckLayerMask(int _iLayerIdx, bool enable)
{
	//강사님 구현 방식
	/*if (m_iLayerMask & 1 << _iLayerIdx)
   {
	   m_iLayerMask &= ~(1 << _iLayerIdx);
   }
   else
   {
	   m_iLayerMask |= 1 << _iLayerIdx;
   }*/
	m_iLayerMask[_iLayerIdx] = enable;
}

void CCamera::CheckLayerMask(const wstring& _strLayerName, bool enable)
{
	const CScene* pScene = CSceneMgr::GetInst()->GetCurScene();
	const CLayer* pLayer = pScene->GetLayer(_strLayerName);
	CheckLayerMask(pLayer->GetLayerIdx(), enable);
}

void CCamera::CalRay()
{
	// 마우스 방향을 향하는 Ray 구하기
	// SwapChain 타겟의 ViewPort 정보
	const CMRT*          pMRT = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN);
	const D3D11_VIEWPORT tVP  = pMRT->GetViewPort();

	//  현재 마우스 좌표
	const Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();

	// 직선은 카메라의 좌표를 반드시 지난다.
	m_ray.vStart = Transform()->GetWorldPos();

	// view space 에서의 방향
	m_ray.vDir.x = ((((vMousePos.x - tVP.TopLeftX) * 2.f / tVP.Width) - 1.f) - m_matProj._31) / m_matProj._11;
	m_ray.vDir.y = (-(((vMousePos.y - tVP.TopLeftY) * 2.f / tVP.Height) - 1.f) - m_matProj._32) / m_matProj._22;
	m_ray.vDir.z = 1.f;

	// world space 에서의 방향
	m_ray.vDir = XMVector3TransformNormal(m_ray.vDir, m_matViewInv);
	m_ray.vDir.Normalize();
}

bool CCamera::FrustumCulling(CGameObject* pObj)
{
	const CBoundingBox* pBoundingBox = pObj->BoundingBox();
	if (!pBoundingBox)
	{
		return true;
	}

	// BB : Bounding Box 
	const Vec3 BB_CenterPos = pBoundingBox->GetWorldPos();
	const Vec3 BB_Size      = pBoundingBox->GetWorldScale();

	// Frustum 내에 Bounding Box 가 있는지 확인 Frustum 안에 있는지 여부 
	const bool IsInFrustum = m_Frustum.CheckBoundingBoxInFrustum(BB_CenterPos, BB_Size);
	// InInFrustum : True -> Render O / False -> Render X
	return !IsInFrustum;
}


void CCamera::SaveToScene(FILE* _pFile)
{
	CComponent::SaveToScene(_pFile);

	fwrite(&m_eProjType, sizeof(UINT), 1, _pFile);

	fwrite(&m_fWidth, sizeof(float), 1, _pFile);
	fwrite(&m_fAspectRatio, sizeof(float), 1, _pFile);
	fwrite(&m_fFOV, sizeof(float), 1, _pFile);
	fwrite(&m_fFar, sizeof(float), 1, _pFile);

	fwrite(&m_iLayerMask, sizeof(UINT), 1, _pFile);
	fwrite(&m_iCamIdx, sizeof(int), 1, _pFile);
}

void CCamera::LoadFromScene(FILE* _pFile)
{
	CComponent::LoadFromScene(_pFile);

	fread(&m_eProjType, sizeof(UINT), 1, _pFile);

	fread(&m_fWidth, sizeof(float), 1, _pFile);
	fread(&m_fAspectRatio, sizeof(float), 1, _pFile);
	fread(&m_fFOV, sizeof(float), 1, _pFile);
	fread(&m_fFar, sizeof(float), 1, _pFile);

	fread(&m_iLayerMask, sizeof(UINT), 1, _pFile);
	fread(&m_iCamIdx, sizeof(int), 1, _pFile);
}

void CCamera::Serialize(YAML::Emitter& emitter)
{
	emitter << YAML::Key << NAME_OF(m_eProjType) << YAML::Value << static_cast<int>(m_eProjType);
	emitter << YAML::Key << NAME_OF(m_fWidth) << YAML::Value << m_fWidth;
	emitter << YAML::Key << NAME_OF(m_fAspectRatio) << YAML::Value << m_fAspectRatio;
	emitter << YAML::Key << NAME_OF(m_fFOV) << YAML::Value << m_fFOV;
	emitter << YAML::Key << NAME_OF(m_fFar) << YAML::Value << m_fFar;
	emitter << YAML::Key << NAME_OF(m_iLayerMask) << YAML::Value << m_iLayerMask.to_ulong();
	emitter << YAML::Key << NAME_OF(m_iCamIdx) << YAML::Value << m_iCamIdx;
}

void CCamera::Deserialize(const YAML::Node& node)
{
	m_eProjType    = static_cast<PROJ_TYPE>(node[NAME_OF(m_eProjType)].as<int>());
	m_fWidth       = node[NAME_OF(m_fWidth)].as<float>();
	m_fAspectRatio = node[NAME_OF(m_fAspectRatio)].as<float>();
	m_fFOV         = node[NAME_OF(m_fFOV)].as<float>();
	m_fFar         = node[NAME_OF(m_fFar)].as<float>();
	m_iLayerMask   = node[NAME_OF(m_iLayerMask)].as<UINT>();
	m_iCamIdx      = node[NAME_OF(m_iCamIdx)].as<int>();
}
