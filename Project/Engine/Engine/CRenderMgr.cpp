#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"
#include "CConstBuffer.h"

#include "CCamera.h"
#include "CResMgr.h"
#include "CLight2D.h"
#include "CLight3D.h"

#include "CStructuredBuffer.h"
#include "CSceneMgr.h"
#include "CScene.h"

#include "CMRT.h"
#include "CRenderEffectMgr.h"
#include "CSSAO.h"
#include "CFXAA.h"
#include "CKeyMgr.h"


CRenderMgr::CRenderMgr()
	: m_pEditorCamera(nullptr)
	, m_pUICamera{nullptr}
	, m_pLight2DBuffer(nullptr)
	, m_pLight3DBuffer(nullptr)
	, m_arrMRT{}
	, m_pMergeShader(nullptr)
	, m_pMergeMaterial(nullptr)
{
	m_pLight2DBuffer = new CStructuredBuffer;
	m_pLight2DBuffer->Create(sizeof(tLightInfo), 2, SB_TYPE::READ_ONLY, true, nullptr);

	m_pLight3DBuffer = new CStructuredBuffer;
	m_pLight3DBuffer->Create(sizeof(tLightInfo), 2, SB_TYPE::READ_ONLY, true, nullptr);
}

CRenderMgr::~CRenderMgr()
{
	SAFE_DELETE(m_pLight2DBuffer);
	SAFE_DELETE(m_pLight3DBuffer);
	Safe_Del_Arr(m_arrMRT);

	SAFE_DELETE(m_pMergeShader);
	SAFE_DELETE(m_pMergeMaterial);
}

void CRenderMgr::update() {}

void CRenderMgr::render()
{
	RenderBegin();

	const CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	pCurScene->GetSceneState() == SCENE_STATE::PLAY ? Render_Play() : Render_Editor();


	if (KEY_TAP(KEY::F1))
		m_bRenderDebugObject = !m_bRenderDebugObject;

	RenderEnd();
}

void CRenderMgr::RenderBegin()
{
	// Rendering 시작	
	ClearAllMRT();

	// TextureRegister 초기화
	ClearTextureRegister();

	// Light 업데이트
	UpdateLight2D();
	UpdateLight3D();

	// Global 상수 업데이트
	static CConstBuffer* pGlobalCB = CDevice::GetInst()->GetCB(CB_TYPE::GLOBAL);
	pGlobalCB->SetData(&g_global, sizeof(tGlobal));
	pGlobalCB->UpdateData();
	pGlobalCB->UpdateData_CS();
}

void CRenderMgr::RenderEnd()
{
	m_vecLight2D.clear();
	m_vecLight3D.clear();
}

void CRenderMgr::Render_MainCamera()
{
	if (m_vecCamera.empty())
	{
		return;
	}

	m_pMainCamera = m_vecCamera[0];
	if (m_pMainCamera)
	{
		m_pMainCamera->SortGameObject();
		Render(MRT_TYPE::SHADOWMAP, m_pMainCamera);

		g_transform.matView    = m_pMainCamera->GetViewMat();
		g_transform.matViewInv = m_pMainCamera->GetViewInvMat();
		g_transform.matProj    = m_pMainCamera->GetProjMat();

		Render(MRT_TYPE::DEFERRED, m_pMainCamera);
		Render(MRT_TYPE::DEFERRED_DECAL, m_pMainCamera);
		Render(MRT_TYPE::SSAO, m_pMainCamera);
		Render(MRT_TYPE::PARTICLE, m_pMainCamera);
		Render(MRT_TYPE::LIGHT, m_pMainCamera);
		Render(MRT_TYPE::SWAPCHAIN, m_pMainCamera);	// Back Buffer
	}

	// 저장된 모든 카메라의 프러스텀을 출력합니다. ( 프러스텀 설정된 카메라만 ) 
	Render_Camera_Frustum();
}

void CRenderMgr::Render_UICamera()
{
	if (!m_pUICamera)
		return;

	m_pUICamera->SortGameObject();
	g_transform.matView    = m_pUICamera->GetViewMat();
	g_transform.matViewInv = m_pUICamera->GetViewInvMat();
	g_transform.matProj    = m_pUICamera->GetProjMat();

	m_arrMRT[static_cast<UINT>(MRT_TYPE::SWAPCHAIN)]->OMSet();
	m_pUICamera->render_forward();				// Foward 물체 렌더링
	m_pUICamera->render_masked();				// Masked 물체 렌더링
	m_pUICamera->render_forward_decal();		// Foward Decal 렌더링
	m_pUICamera->render_translucent();			// Alpha 물체 렌더링
	m_pUICamera->render_debug();				// Debug Object Render
	m_pUICamera->render_postprocess();			// PostProcess 물체 렌더링
}


void CRenderMgr::Render_Play()
{
	// 여러 카메라 중 메인 카메라로 장면을 찍는다.
	Render_MainCamera();
	Render_UICamera();

	// Post Process 
	CRenderEffectMgr::GetInst()->Apply(EFFECT_TYPE::FXAA);
	CRenderEffectMgr::GetInst()->Apply(EFFECT_TYPE::FADE_IN_PAPERBURN);
	CRenderEffectMgr::GetInst()->Apply(EFFECT_TYPE::FADE_OUT_PAPERBURN);
}

void CRenderMgr::Render_Editor()
{
	if (nullptr == m_pEditorCamera)
		return;

	m_pEditorCamera->SortGameObject();
	Render(MRT_TYPE::SHADOWMAP, m_pEditorCamera);

	g_transform.matView    = m_pEditorCamera->GetViewMat();
	g_transform.matViewInv = m_pEditorCamera->GetViewInvMat();
	g_transform.matProj    = m_pEditorCamera->GetProjMat();

	Render(MRT_TYPE::DEFERRED, m_pEditorCamera);
	Render(MRT_TYPE::DEFERRED_DECAL, m_pEditorCamera);
	Render(MRT_TYPE::PARTICLE, m_pEditorCamera);
	Render(MRT_TYPE::LIGHT, m_pEditorCamera);
	Render(MRT_TYPE::SWAPCHAIN, m_pEditorCamera);
}

void CRenderMgr::Render(MRT_TYPE _eMRT, CCamera* _pCam)
{
	// MRT Setting 
	m_arrMRT[static_cast<UINT>(_eMRT)]->OMSet();

	// MRT Render 
	switch (_eMRT)
	{
	case MRT_TYPE::SWAPCHAIN:
		{
			Ptr<CMesh> pRectMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
			m_pMergeMaterial->UpdateData();
			pRectMesh->render(0);

			_pCam->render_forward();			// Foward 물체 렌더링
			_pCam->render_masked();				// Masked 물체 렌더링
			_pCam->render_forward_decal();		// Foward Decal 렌더링
			_pCam->render_translucent();		// Alpha 물체 렌더링
			if (m_bRenderDebugObject)
				_pCam->render_debug();				// Debug Object Render
			_pCam->render_postprocess();		// PostProcess 물체 렌더링
		}
		break;
	case MRT_TYPE::DEFERRED:
		{
			_pCam->render_deferred();
		}
		break;
	case MRT_TYPE::PARTICLE:
		{
			_pCam->render_particle();
		}
		break;
	case MRT_TYPE::DEFERRED_DECAL:
		{
			_pCam->render_deferred_decal();
		}
		break;
	case MRT_TYPE::LIGHT:
		{
			for (const auto& pLight3D : m_vecLight3D)
			{
				pLight3D->render();
			}
		}
		break;
	case MRT_TYPE::SHADOWMAP:
		{
			Render_ShadowMap(LIGHT_TYPE::DIRECTIONAL);
		}
		break;
	case MRT_TYPE::SSAO:
		{
			CRenderEffectMgr::GetInst()->Apply(EFFECT_TYPE::SSAO);
		}
		break;
	}
}

void CRenderMgr::Render_ShadowMap(LIGHT_TYPE _eLightType) const
{
	m_arrMRT[static_cast<UINT>(MRT_TYPE::SHADOWMAP)]->OMSet();

	for (const auto& pLight3D : m_vecLight3D)
	{
		if (_eLightType == pLight3D->GetLightType())
		{
			pLight3D->render_shadowmap();
		}
	}
}

void CRenderMgr::Render_Lights() const
{
	m_arrMRT[static_cast<UINT>(MRT_TYPE::LIGHT)]->OMSet();
}

void CRenderMgr::Render_Camera_Frustum()
{
	// 현재 저장된 카메라의 모든 프러스텀을 출력합니다. 
	for (int i = 1; i < m_vecCamera.size(); ++i)
	{
		if (m_vecCamera[i]->GetShowFrustum())
		{
			m_vecCamera[i]->render_Frustum();
		}
	}
}

void CRenderMgr::RegisterCamera(CCamera* _pCamera)
{
	// 카메라가 RenderMgr에 최초 등록 되는 경우
	if (-1 == _pCamera->m_iCamIdx)
	{
		m_vecCamera.push_back(_pCamera);
		const int cameraIndex = static_cast<int>(m_vecCamera.size() - 1);
		_pCamera->m_iCamIdx   = cameraIndex;
	}
	else
	{
		if (m_vecCamera.size() <= _pCamera->m_iCamIdx)
		{
			m_vecCamera.resize(static_cast<size_t>(_pCamera->m_iCamIdx + 1));
		}

		m_vecCamera[_pCamera->m_iCamIdx] = _pCamera;
	}
}

void CRenderMgr::SwapCameraIndex(CCamera* _pTargetCamera, int _iCameraIndexToChange) const
{
	for (size_t cameraIndex = 0; cameraIndex < m_vecCamera.size(); ++cameraIndex)
	{
		if (_pTargetCamera == m_vecCamera[cameraIndex])
		{
			if (nullptr != m_vecCamera[_iCameraIndexToChange])
			{
				m_vecCamera[_iCameraIndexToChange]->m_iCamIdx = static_cast<int>(cameraIndex);
				_pTargetCamera->m_iCamIdx                     = _iCameraIndexToChange;

				return;
			}
		}
	}

	assert(nullptr);
}

void CRenderMgr::CopyTargetToPostProcess()
{
	Ptr<CTexture> pRenderTarget = CResMgr::GetInst()->FindRes<CTexture>(L"RenderTargetTex");
	Ptr<CTexture> pPostProcess  = CResMgr::GetInst()->FindRes<CTexture>(L"PostProcessTex");

	CONTEXT->CopyResource(pPostProcess->GetTex2D().Get(), pRenderTarget->GetTex2D().Get());
}

void CRenderMgr::UpdateLight2D() const
{
	//구조화버퍼로 값을 넘기므로, 크기가 작다면 재 생성
	if (m_pLight2DBuffer->GetElementCount() < m_vecLight2D.size())
	{
		m_pLight2DBuffer->Create(static_cast<UINT>(sizeof(tLightInfo)),
		                         static_cast<UINT>(m_vecLight2D.size()),
		                         SB_TYPE::READ_ONLY,
		                         true,
		                         nullptr);
	}

	//구조화 버퍼에 값을 갱신
	static vector<tLightInfo> vecLight2DInfo{};
	vecLight2DInfo.clear();

	for (const auto& pLight2D : m_vecLight2D)
	{
		vecLight2DInfo.push_back(pLight2D->GetLightInfo());
	}
	m_pLight2DBuffer->SetData(vecLight2DInfo.data(), static_cast<UINT>(vecLight2DInfo.size()));
	m_pLight2DBuffer->UpdateData(PS, 60);

	//글로벌 데이터 갱신
	g_global.Light2DCount = static_cast<int>(m_vecLight2D.size());
}

void CRenderMgr::UpdateLight3D() const
{
	//구조화버퍼로 값을 넘기므로, 크기가 작다면 재 생성
	if (m_pLight3DBuffer->GetElementCount() < m_vecLight3D.size())
	{
		m_pLight3DBuffer->Create(static_cast<UINT>(sizeof(tLightInfo)),
		                         static_cast<UINT>(m_vecLight3D.size()),
		                         SB_TYPE::READ_ONLY,
		                         true,
		                         nullptr);
	}

	//구조화버퍼에 값을 세팅
	static vector<tLightInfo> vecLight3DInfo{};
	vecLight3DInfo.clear();

	for (const auto& pLight3D : m_vecLight3D)
	{
		vecLight3DInfo.push_back(pLight3D->GetLightInfo());
	}
	m_pLight3DBuffer->SetData(vecLight3DInfo.data(), static_cast<UINT>(vecLight3DInfo.size()));
	m_pLight3DBuffer->UpdateData(PS, 61);

	//글로벌 데이터 갱신
	g_global.Light3DCount = static_cast<int>(m_vecLight3D.size());
}

// 현재 시점 카메라 가져오기
CCamera* CRenderMgr::GetMainCam() const
{
	const CScene* pScene = CSceneMgr::GetInst()->GetCurScene();
	if (nullptr == pScene)
	{
		return nullptr;
	}

	if (pScene->GetSceneState() == SCENE_STATE::PLAY)
	{
		if (m_vecCamera.empty())
		{
			return nullptr;
		}
		return m_vecCamera[0];
	}
	return m_pEditorCamera;
}

CCamera* CRenderMgr::GetUICamera()
{
	if (nullptr == m_pUICamera)
	{
		if (CSceneMgr::GetInst()->GetCurScene()->GetSceneState() != SCENE_STATE::PLAY)
		{
			return m_pEditorCamera;
		}

		const auto iter = std::find_if(m_vecCamera.begin(),
		                               m_vecCamera.end(),
		                               [](const CCamera* pCamera)
		                               {
			                               return pCamera->GetOwner()->GetName() == L"UICamera";
		                               });
		if (iter != m_vecCamera.end())
		{
			m_pUICamera = *iter;
			return m_pUICamera;
		}
		assert(m_pUICamera && "UI CAMERA NOT FOUND");
	}
	return m_pUICamera;
}


CCamera* CRenderMgr::GetCameraByName(wstring _name)
{
	for (int i = 0; i < m_vecCamera.size(); ++i)
	{
		if (m_vecCamera[i]->GetOwner()->GetName() == _name)
			return m_vecCamera[i];
	}

	return nullptr;
}
