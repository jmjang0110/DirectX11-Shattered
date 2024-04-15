#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"
#include "CResMgr.h"

#include "CMRT.h"
#include "CGraphicsShader.h"
#include "CMaterial.h"

namespace { }

void CRenderMgr::init()
{
	// PostProcess Texture 만들기
	const Vec2    scrreenResolution = CDevice::GetInst()->GetRenderResolution();
	Ptr<CTexture> pPostProcessTex   = CResMgr::GetInst()->CreateTexture(L"PostProcessTex",
	                                                                    static_cast<UINT>(scrreenResolution.x),
	                                                                    static_cast<UINT>(scrreenResolution.y),
	                                                                    DXGI_FORMAT_R8G8B8A8_UNORM,
	                                                                    D3D11_BIND_SHADER_RESOURCE,
	                                                                    true);
	CreateMRT();
	CreateMaterial();
}

void CRenderMgr::CreateMRT()
{
	CreateSwapChainMRT();
	CreateDefferedMRT();
	CreateParticleMRT();
	CreateDeferredDecalMRT();
	CreateLightMRT();
	CreateShadowMapMRT();
	CreateSSAOMRT();
}

void CRenderMgr::CreateMaterial()
{
	CreateMergeMaterial();
	CreateDirectionalLightMaterial();
	CreatePointLightMaterial();
	CreateSpotLightMaterial();
	CreateShadowMapMaterial();
	CreateGridMaterial();
	CreateDefferedDecalMaterial();
	CreateForwardDecalMaterial();
}

void CRenderMgr::ClearAllMRT() const
{
	for (auto& pMRT : m_arrMRT)
	{
		if (nullptr != pMRT)
		{
			pMRT->Clear();
		}
	}
}

void CRenderMgr::ClearMRT(MRT_TYPE _eType) const
{
	CMRT* pMrt = m_arrMRT[static_cast<UINT>(_eType)];
	if (pMrt)
		pMrt->Clear();

}

void CRenderMgr::ClearTextureRegister()
{
	for (UINT i = 0; i < static_cast<UINT>(TEX_PARAM::END); ++i)
	{
		CTexture::Clear(i);
	}
}

void CRenderMgr::CreateSwapChainMRT()
{
	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN] = new CMRT{};

	Ptr<CTexture>       pRenderTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"RenderTargetTex");
	const Ptr<CTexture> pDepthStencilTex = CResMgr::GetInst()->FindRes<CTexture>(L"DepthStencilTex");
	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->Create(1, &pRenderTargetTex, pDepthStencilTex);
}

void CRenderMgr::CreateDefferedMRT()
{
	const Vec2          vResolution      = CDevice::GetInst()->GetRenderResolution();
	const Ptr<CTexture> pDepthStencilTex = CResMgr::GetInst()->FindRes<CTexture>(L"DepthStencilTex");
	Ptr<CTexture>       arrTex[8]        = {
		CResMgr::GetInst()->CreateTexture(L"ColorTargetTex",
		                                  static_cast<UINT>(vResolution.x),
		                                  static_cast<UINT>(vResolution.y),
		                                  DXGI_FORMAT_R8G8B8A8_UNORM,
		                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		                                  true),

		CResMgr::GetInst()->CreateTexture(L"NormalTargetTex",
		                                  static_cast<UINT>(vResolution.x),
		                                  static_cast<UINT>(vResolution.y),
		                                  DXGI_FORMAT_R32G32B32A32_FLOAT,
		                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		                                  true),

		CResMgr::GetInst()->CreateTexture(L"PositionTargetTex",
		                                  static_cast<UINT>(vResolution.x),
		                                  static_cast<UINT>(vResolution.y),
		                                  DXGI_FORMAT_R32G32B32A32_FLOAT,
		                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		                                  true),

		CResMgr::GetInst()->CreateTexture(L"DataTargetTex",
		                                  static_cast<UINT>(vResolution.x),
		                                  static_cast<UINT>(vResolution.y),
		                                  DXGI_FORMAT_R32G32B32A32_FLOAT,
		                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		                                  true),

	};

	m_arrMRT[(UINT)MRT_TYPE::DEFERRED] = new CMRT{};
	m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->Create(4, arrTex, pDepthStencilTex);
}

void CRenderMgr::CreateParticleMRT()
{
	const Vec2          vResolution      = CDevice::GetInst()->GetRenderResolution();
	const Ptr<CTexture> pDepthStencilTex = CResMgr::GetInst()->FindRes<CTexture>(L"DepthStencilTex");
	Ptr<CTexture>       arrTex[8]
		= {
			CResMgr::GetInst()->CreateTexture(L"ParticleTargetTex",
			                                  static_cast<UINT>(vResolution.x),
			                                  static_cast<UINT>(vResolution.y),
			                                  DXGI_FORMAT_R32G32B32A32_FLOAT,
			                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
			                                  true),

			CResMgr::GetInst()->CreateTexture(L"EmissiveTargetTex",
			                                  static_cast<UINT>(vResolution.x),
			                                  static_cast<UINT>(vResolution.y),
			                                  DXGI_FORMAT_R32G32B32A32_FLOAT,
			                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
			                                  true)
		};

	m_arrMRT[(UINT)(MRT_TYPE::PARTICLE)] = new CMRT{};
	m_arrMRT[(UINT)(MRT_TYPE::PARTICLE)]->Create(2, arrTex, pDepthStencilTex);
}

void CRenderMgr::CreateDeferredDecalMRT()
{
	Ptr<CTexture> arrTex[8] = {CResMgr::GetInst()->FindRes<CTexture>(L"ColorTargetTex")};

	m_arrMRT[(UINT)MRT_TYPE::DEFERRED_DECAL] = new CMRT{};
	m_arrMRT[(UINT)MRT_TYPE::DEFERRED_DECAL]->Create(1, arrTex, nullptr);
}

void CRenderMgr::CreateLightMRT()
{
	const Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();

	Ptr<CTexture> arrTex[8] = {
		CResMgr::GetInst()->CreateTexture(L"DiffuseTargetTex",
		                                  static_cast<UINT>(vResolution.x),
		                                  static_cast<UINT>(vResolution.y),
		                                  DXGI_FORMAT_R8G8B8A8_UNORM,
		                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		                                  true),

		CResMgr::GetInst()->CreateTexture(L"SpecularTargetTex",
		                                  static_cast<UINT>(vResolution.x),
		                                  static_cast<UINT>(vResolution.y),
		                                  DXGI_FORMAT_R8G8B8A8_UNORM,
		                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		                                  true),

		CResMgr::GetInst()->CreateTexture(L"ShadowPowerTargetTex",
		                                  static_cast<UINT>(vResolution.x),
		                                  static_cast<UINT>(vResolution.y),
		                                  DXGI_FORMAT_R32_FLOAT,
		                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		                                  true),
	};

	m_arrMRT[(UINT)MRT_TYPE::LIGHT] = new CMRT{};
	m_arrMRT[(UINT)MRT_TYPE::LIGHT]->Create(3, arrTex, nullptr);
}

void CRenderMgr::CreateShadowMapMRT()
{
	constexpr int textureSize = 4096;
	Ptr<CTexture> arrTex[8]   = {
		CResMgr::GetInst()->CreateTexture(L"ShadowMapTex",
		                                  textureSize,
		                                  textureSize,
		                                  DXGI_FORMAT_R32_FLOAT,
		                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		                                  true),
	};

	// Depth Stencil Texture 만들기
	Ptr<CTexture> pDepthStencilTex = CResMgr::GetInst()->CreateTexture(L"ShadowMapDSTex",
	                                                                   textureSize,
	                                                                   textureSize,
	                                                                   DXGI_FORMAT_D32_FLOAT,
	                                                                   D3D11_BIND_DEPTH_STENCIL,
	                                                                   true);

	m_arrMRT[(UINT)MRT_TYPE::SHADOWMAP] = new CMRT{};
	m_arrMRT[(UINT)MRT_TYPE::SHADOWMAP]->Create(1, arrTex, pDepthStencilTex);
}

void CRenderMgr::CreateSSAOMRT()
{
	// ===========================================
	// SSAO MRT ( Screen Space Ambient Occlusion )
	// ===========================================
	const Vec2          vResolution      = CDevice::GetInst()->GetRenderResolution();
	const Ptr<CTexture> pDepthStencilTex = CResMgr::GetInst()->FindRes<CTexture>(L"DepthStencilTex");

	Ptr<CTexture> arrTex[8]
		= {
			CResMgr::GetInst()->CreateTexture(L"AmbientOcclusionTex",
			                                  (UINT)vResolution.x,
			                                  (UINT)vResolution.y,
			                                  DXGI_FORMAT_R32G32B32A32_FLOAT,
			                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
			                                  true),

			CResMgr::GetInst()->CreateTexture(L"AmbientOcclusionTempTex",
			                                  (UINT)vResolution.x,
			                                  (UINT)vResolution.y,
			                                  DXGI_FORMAT_R32G32B32A32_FLOAT,
			                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
			                                  true),

			CResMgr::GetInst()->CreateTexture(L"RandomVectorTex",
			                                  (UINT)vResolution.x,
			                                  (UINT)vResolution.y,
			                                  DXGI_FORMAT_R32G32B32A32_FLOAT,
			                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
			                                  true),


		};

	m_arrMRT[(UINT)MRT_TYPE::SSAO] = new CMRT;
	m_arrMRT[(UINT)MRT_TYPE::SSAO]->Create(3, arrTex, pDepthStencilTex);
}

void CRenderMgr::CreateMergeMaterial()
{
	// Merge Shader
	m_pMergeShader = new CGraphicsShader;
	m_pMergeShader->CreateVertexShader(L"shader\\light.fx", "VS_Merge");
	m_pMergeShader->CreatePixelShader(L"shader\\light.fx", "PS_Merge");
	m_pMergeShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_NONE);
	m_pMergeShader->SetRSType(RS_TYPE::CULL_BACK);
	m_pMergeShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	m_pMergeShader->SetBSType(BS_TYPE::DEFAULT);

	// Merge Material
	m_pMergeMaterial = new CMaterial;
	m_pMergeMaterial->SetShader(m_pMergeShader);
	m_pMergeMaterial->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"ColorTargetTex"));
	m_pMergeMaterial->SetTexParam(TEX_PARAM::TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex"));
	m_pMergeMaterial->SetTexParam(TEX_PARAM::TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"SpecularTargetTex"));
	m_pMergeMaterial->SetTexParam(TEX_PARAM::TEX_3, CResMgr::GetInst()->FindRes<CTexture>(L"ShadowPowerTargetTex"));
	m_pMergeMaterial->SetTexParam(TEX_PARAM::TEX_4, CResMgr::GetInst()->FindRes<CTexture>(L"AmbientOcclusionTex"));
	m_pMergeMaterial->SetTexParam(TEX_PARAM::TEX_5, CResMgr::GetInst()->FindRes<CTexture>(L"EmissiveTargetTex"));
}

void CRenderMgr::CreateDirectionalLightMaterial()
{
	// Directional Light Shader
	Ptr<CGraphicsShader> pShader = new CGraphicsShader;
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_LIGHT);
	pShader->CreateVertexShader(L"Shader\\light.fx", "VS_Directional");
	pShader->CreatePixelShader(L"Shader\\light.fx", "PS_Directional");

	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::ONEONE_BLEND);

	CResMgr::GetInst()->AddRes<CGraphicsShader>(L"DirLightShader", pShader.Get(), true);

	// Directional Light Material
	Ptr<CMaterial> pMtrl = new CMaterial;
	pMtrl->SetShader(pShader);
	pMtrl->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	pMtrl->SetTexParam(TEX_PARAM::TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex"));
	pMtrl->SetTexParam(TEX_PARAM::TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"ShadowMapTex"));

	Ptr<CTexture> pShadowMapTex  = CResMgr::GetInst()->FindRes<CTexture>(L"ShadowMapTex");
	auto          vShadowMapSize = Vec2{pShadowMapTex->Width(), pShadowMapTex->Height()};
	pMtrl->SetScalarParam(SCALAR_PARAM::VEC2_0, &vShadowMapSize);
	float fEpsilon = 0.001f;
	pMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_0, &fEpsilon);

	CResMgr::GetInst()->AddRes<CMaterial>(L"material\\DirLightMtrl.mtrl", pMtrl.Get(), true);
}

void CRenderMgr::CreatePointLightMaterial()
{
	// Point Light Shader
	Ptr<CGraphicsShader> pShader = new CGraphicsShader;
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_LIGHT);
	pShader->CreateVertexShader(L"Shader\\light.fx", "VS_Point");
	pShader->CreatePixelShader(L"Shader\\light.fx", "PS_Point");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::ONEONE_BLEND);

	CResMgr::GetInst()->AddRes<CGraphicsShader>(L"PointLightShader", pShader.Get(), true);

	// Point Light Material
	Ptr<CMaterial> pMtrl = new CMaterial;
	pMtrl->SetShader(pShader);
	pMtrl->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	pMtrl->SetTexParam(TEX_PARAM::TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex"));

	CResMgr::GetInst()->AddRes<CMaterial>(L"material\\PointLightMtrl.mtrl", pMtrl.Get(), true);
}

void CRenderMgr::CreateSpotLightMaterial()
{
	// Spot Light Shader
	Ptr<CGraphicsShader> pShader = new CGraphicsShader;
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_LIGHT);
	pShader->CreateVertexShader(L"Shader\\light.fx", "VS_Spot");
	pShader->CreatePixelShader(L"Shader\\light.fx", "PS_Spot");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::ONEONE_BLEND);

	CResMgr::GetInst()->AddRes<CGraphicsShader>(L"SpotLightShader", pShader.Get(), true);

	// Spot Light Material
	Ptr<CMaterial> pMtrl = new CMaterial;
	pMtrl->SetShader(pShader);

	pMtrl->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	pMtrl->SetTexParam(TEX_PARAM::TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex"));

	CResMgr::GetInst()->AddRes<CMaterial>(L"material\\SpotLightMtrl.mtrl", pMtrl.Get(), true);
}

void CRenderMgr::CreateShadowMapMaterial()
{
	Ptr<CGraphicsShader> pShader = new CGraphicsShader;
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_NONE);
	pShader->CreateVertexShader(L"Shader\\light.fx", "VS_ShadowMap");
	pShader->CreatePixelShader(L"Shader\\light.fx", "PS_ShadowMap");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);

	CResMgr::GetInst()->AddRes<CGraphicsShader>(L"ShadowMapShader", pShader.Get(), true);

	// ShadowMap Material
	Ptr<CMaterial> pMtrl = new CMaterial;
	pMtrl->SetShader(pShader);

	CResMgr::GetInst()->AddRes<CMaterial>(L"material\\ShadowMap.mtrl", pMtrl.Get(), true);
}

void CRenderMgr::CreateGridMaterial()
{
	Ptr<CGraphicsShader> pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"Shader\\tool.fx", "VS_Grid");
	pShader->CreatePixelShader(L"Shader\\tool.fx", "PS_Grid");

	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TOOL);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetDSType(DS_TYPE::NO_WRITE);

	// g_int_0 : 간격 단계
	// g_float_0 : 카메라 높이에 따른 그리드의 투명도
	// g_float_1 : 선의 두께
	// g_vec4_0  : 카메라 위치
	// g_vec4_1  : Grid 색상
	// g_tex_0  : Position Target
	pShader->AddScalarParamInfo(L"Line Thickness", SCALAR_PARAM::FLOAT_1);
	pShader->AddScalarParamInfo(L"Grid Color", SCALAR_PARAM::VEC4_1);
	pShader->AddTexParamInfo(L"Position Target", TEX_PARAM::TEX_0);

	CResMgr::GetInst()->AddRes<CGraphicsShader>(L"GridShader", pShader.Get(), true);

	// Grid Material
	Ptr<CMaterial> pMtrl = new CMaterial;
	pMtrl->SetShader(pShader);

	pMtrl->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));

	CResMgr::GetInst()->AddRes<CMaterial>(L"material\\GridMtrl.mtrl", pMtrl.Get(), true);
}

void CRenderMgr::CreateDefferedDecalMaterial()
{
	Ptr<CGraphicsShader> pShader = new CGraphicsShader;

	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_DEFERRED_DECAL);
	pShader->CreateVertexShader(L"Shader\\std3d_deferred.fx", "VS_Deferred_Decal");
	pShader->CreatePixelShader(L"Shader\\std3d_deferred.fx", "PS_Deferred_Decal");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	CResMgr::GetInst()->AddRes<CGraphicsShader>(L"DeferredDecalShader", pShader.Get(), true);

	// Deferred Decal Material
	Ptr<CMaterial> pMtrl = new CMaterial;
	pMtrl->SetShader(pShader);
	pMtrl->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	CResMgr::GetInst()->AddRes<CMaterial>(L"material\\DeferredDecalMtrl.mtrl", pMtrl.Get(), true);
}

void CRenderMgr::CreateForwardDecalMaterial()
{
	// Forward Decal Shader
	Ptr<CGraphicsShader> pShader = new CGraphicsShader;

	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_FORWARD_DECAL);
	pShader->CreateVertexShader(L"Shader\\std3d_deferred.fx", "VS_Deferred_Decal");
	pShader->CreatePixelShader(L"Shader\\std3d_deferred.fx", "PS_Deferred_Decal");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	CResMgr::GetInst()->AddRes<CGraphicsShader>(L"ForwardDecalShader", pShader.Get(), true);

	// Forward Decal Material
	Ptr<CMaterial> pMtrl = new CMaterial;
	pMtrl->SetShader(pShader);
	pMtrl->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	CResMgr::GetInst()->AddRes<CMaterial>(L"material\\ForwardDecalMtrl.mtrl", pMtrl.Get(), true);
}
