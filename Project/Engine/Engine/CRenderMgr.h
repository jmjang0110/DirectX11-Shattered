#pragma once
#include "singleton.h"

class CCamera;
class CLight2D;
class CLight3D;
class CMRT;

class CRenderMgr
	: public CSingleton<CRenderMgr>
{
	SINGLE(CRenderMgr);
private:
	CCamera*			m_pMainCamera;			// 현재 시점 카메라
	vector<CCamera*>	m_vecCamera;			// Scene 에 있는 Camera 들
	CCamera*			m_pEditorCamera;		// Editor 시점 카메라
	CCamera*			m_pUICamera;

	vector<CLight2D*>	m_vecLight2D;
	CStructuredBuffer*	m_pLight2DBuffer;		// Scene 의 2D 광원 버퍼

	vector<CLight3D*>	m_vecLight3D;
	CStructuredBuffer*	m_pLight3DBuffer;		// Scene 의 3D 광원 버퍼

	std::array<CMRT*, static_cast<UINT>(MRT_TYPE::END)> m_arrMRT;

	// Merge 
	CGraphicsShader*	m_pMergeShader;
	CMaterial*			m_pMergeMaterial;

	bool m_bRenderDebugObject;

private:
	// [ CREATE ]
	void CreateSwapChainMRT();
	void CreateDefferedMRT();
	void CreateParticleMRT();
	void CreateDeferredDecalMRT();
	void CreateLightMRT();
	void CreateShadowMapMRT();
	void CreateSSAOMRT();


	void CreateMergeMaterial();
	void CreateDirectionalLightMaterial();
	void CreatePointLightMaterial();
	void CreateSpotLightMaterial();
	void CreateShadowMapMaterial();
	void CreateGridMaterial();
	void CreateDefferedDecalMaterial();
	void CreateForwardDecalMaterial();

	// [ RENDER ]
	void RenderBegin();
	void RenderEnd();
	void Render_MainCamera();
	void Render_UICamera();
	void Render(MRT_TYPE _eMRT, CCamera* _pCam);

	void Render_Play();
	void Render_Editor();
	void Render_ShadowMap(LIGHT_TYPE _eLightType) const;
	void Render_Lights() const;
	void Render_Camera_Frustum();


public:
	void init();
	void update();
	void render();

public:
	void RegisterCamera(CCamera* _pCamera);
	void RegisterEditorCamera(CCamera* _pCam) { m_pEditorCamera = _pCam; }

	void ClearCamera() { m_vecCamera.clear(); }
	void SwapCameraIndex(CCamera* _pTargetCamera, int _iCameraIndexToChange) const;
	void CopyTargetToPostProcess();

	int RegisterLight2D(CLight2D* _pLight2D)
	{
		m_vecLight2D.push_back(_pLight2D);
		return static_cast<int>(m_vecLight2D.size()) - 1;
	}

	int RegisterLight3D(CLight3D* _pLight3D)
	{
		m_vecLight3D.push_back(_pLight3D);
		return static_cast<int>(m_vecLight3D.size()) - 1;
	}


	// 현재 시점 카메라 가져오기
	CCamera* GetMainCam() const;
	CCamera* GetUICamera();
	CMRT*    GetMRT(MRT_TYPE _eType) const { return m_arrMRT[static_cast<UINT>(_eType)]; }
	CCamera* GetCameraByName(wstring _name);


private:
	void CreateMRT();
	void CreateMaterial();
	void ClearAllMRT() const;
	void ClearMRT(MRT_TYPE _eType) const;
	void ClearTextureRegister();

	void UpdateLight2D() const;
	void UpdateLight3D() const;
};
