#include "pch.h"
#include "CSSAO.h"

#include "CCamera.h"
#include "CRenderMgr.h"
#include "CConstBuffer.h"
#include <random>
#include "CDevice.h"


CSSAO::CSSAO()
	: m_tSSAO_ChangesRarely{}
	, m_tViewPort{} {}

CSSAO::~CSSAO() = default;

void CSSAO::Init()
{
	Init_SSAO_Texture();
	BuildOffsetVectors();
	BuildRandomVectorTexture();

	// 마터리얼 초기회 
	Init_SSAO_NormalDepthMtrl();
	Init_SSAO_Mtrl();
	Init_SSAO_BlurHorzMtrl();
	Init_SSAO_BlurVertMtrl();
	Init_SSAO_DebugMtrl();
}

void CSSAO::Apply()
{
	Begin();
	UpdateData_SSAO_ChangesOnResize();
	UpdateData_SSAO_ChangesRarely();
	End();

	RenderToSSAOTexture();
}

void CSSAO::Init_SSAO_Texture() {}

void CSSAO::Init_SSAO_NormalDepthMtrl()
{
	const auto pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\SSAO.fx", "SSAO_GeometryVS");
	pShader->CreatePixelShader(L"shader\\SSAO.fx", "SSAO_GeometryPS");
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CResMgr::GetInst()->AddRes<CGraphicsShader>(L"SSAOGeometryShader", pShader, true);

	// MATERIAL
	const auto pMtrl = new CMaterial;
	pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"SSAOGeometryShader"));
	CResMgr::GetInst()->AddRes<CMaterial>(L"material\\SSAO_Geometrymtrl.mtrl", pMtrl);
}

void CSSAO::Init_SSAO_Mtrl()
{
	const auto pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\SSAO.fx", "SSAO_VS");
	pShader->CreatePixelShader(L"shader\\SSAO.fx", "SSAO_PS");
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CResMgr::GetInst()->AddRes<CGraphicsShader>(L"SSAOShader", pShader, true);

	// MATERIAL
	const auto pMtrl = new CMaterial;
	pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"SSAOShader"));
	CResMgr::GetInst()->AddRes<CMaterial>(L"material\\SSAOmtrl.mtrl", pMtrl);
}


void CSSAO::Init_SSAO_BlurHorzMtrl()
{
	const auto pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\FullScreenTriangle.fx", "VS_FullScreenTriangleTexcoord");
	pShader->CreatePixelShader(L"shader\\SSAO.fx", "BilateralPS");
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CResMgr::GetInst()->AddRes<CGraphicsShader>(L"SSAO_BlurHorzShader", pShader, true);

	// MATERIAL
	const Ptr<CTexture> pNormalDepthTex = CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex");
	const Ptr<CTexture> pAOTex          = CResMgr::GetInst()->FindRes<CTexture>(L"AmbientOcclusionTex");

	const auto pMtrl = new CMaterial;
	pMtrl->SetTexParam(TEX_PARAM::TEX_1, pNormalDepthTex);
	pMtrl->SetTexParam(TEX_PARAM::TEX_3, pAOTex);
	pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"SSAO_BlurHorzShader"));
	CResMgr::GetInst()->AddRes<CMaterial>(L"material\\SSAOBlurHorzmtrl.mtrl", pMtrl);
}

void CSSAO::Init_SSAO_BlurVertMtrl()
{
	const auto pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\FullScreenTriangle.fx", "VS_FullScreenTriangleTexcoord");
	pShader->CreatePixelShader(L"shader\\SSAO.fx", "BilateralPS");
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CResMgr::GetInst()->AddRes<CGraphicsShader>(L"SSAO_BlurVertShader", pShader, true);

	// MATERIAL
	const Ptr<CTexture> pNormalDepthTex = CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex");
	const Ptr<CTexture> pAOTex          = CResMgr::GetInst()->FindRes<CTexture>(L"AmbientOcclusionTex");

	const auto pMtrl = new CMaterial;
	pMtrl->SetTexParam(TEX_PARAM::TEX_1, pNormalDepthTex);
	pMtrl->SetTexParam(TEX_PARAM::TEX_3, pAOTex);
	pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"SSAO_BlurVertShader"));
	CResMgr::GetInst()->AddRes<CMaterial>(L"material\\SSAOBlurVertmtrl.mtrl", pMtrl);
}

void CSSAO::Init_SSAO_DebugMtrl()
{
	const auto pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\FullScreenTriangle.fx", "VS_FullScreenTriangleTexcoord");
	pShader->CreatePixelShader(L"shader\\SSAO.fx", "DebugAO_PS");
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CResMgr::GetInst()->AddRes<CGraphicsShader>(L"SSAODebugShader", pShader, true);

	// MATERIAL
	const auto pMtrl = new CMaterial;
	pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"SSAODebugShader"));
	CResMgr::GetInst()->AddRes<CMaterial>(L"material\\SSAO_DebugAOmtrl.mtrl", pMtrl);
}

void CSSAO::UpdateData_SSAO_ChangesOnResize()
{
	const CCamera* pCamera = CRenderMgr::GetInst()->GetMainCam();

	const float zFar       = pCamera->GetFar();
	const float halfHeight = zFar * tanf(0.5f * pCamera->GetFOV());
	const float halfWidth  = pCamera->GetAspectRatio() * halfHeight;

	// 단일 삼각형 렌더링, 제공된 먼 평면 포인트는 다음을 충족합니다.
	// (-1, 1)--(3, 1)
	//   |     /
	//   |    /
	//   |   /       
	// (-1, -3)
	const Vec4 farPlanePoints[3] =
	{
		Vec4(-halfWidth, halfHeight, zFar, 0.0f),
		Vec4(3.0f * halfWidth, halfHeight, zFar, 0.0f),
		Vec4(-halfWidth, -3.0f * halfHeight, zFar, 0.0f),
	};

	memcpy_s(&m_tSSAO_ChangesOnResize.g_FarPlanePoints[0], sizeof(Vec4) * 3, &farPlanePoints[0], sizeof(Vec4) * 3);

	// NDC 공간 [-1, 1]^2에서 텍스처 공간 [0, 1]^2으로 변환
	static const auto T = Matrix
	{
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f
	};
	const Matrix matProjection = CRenderMgr::GetInst()->GetMainCam()->GetProjMat();
	// 뷰 공간에서 텍스처 공간으로 변환
	Matrix PT                                = matProjection * T;
	PT                                       = XMMatrixTranspose(PT);
	m_tSSAO_ChangesOnResize.g_ViewToTexSpace = PT;


	Ptr<CTexture> pAOTempTex            = CResMgr::GetInst()->FindRes<CTexture>(L"AmbientOcclusionTempTex");
	const auto    AOViewportSize        = Vec2(1.f / pAOTempTex->Width(), 1.f / pAOTempTex->Height());
	m_tSSAO_ChangesOnResize.g_TexelSize = AOViewportSize;

	// CPU -> GPU [ register(b4) ] 
	CConstBuffer* pBuffer = CDevice::GetInst()->GetCB(CB_TYPE::SSAO_CHANGES_ON_RESIZE);
	pBuffer->SetData(&m_tSSAO_ChangesOnResize, sizeof(tSSAO_ChangesOnResize));
	pBuffer->UpdateData();
}

void CSSAO::UpdateData_SSAO_ChangesRarely()
{
	m_tSSAO_ChangesRarely.g_OcclusionRadius    = m_OcclusionRadius;
	m_tSSAO_ChangesRarely.g_OcclusionFadeStart = m_OcclusionFadeStart;
	m_tSSAO_ChangesRarely.g_OcclusionFadeEnd   = m_OcclusionFadeEnd;

	memcpy_s(&m_tSSAO_ChangesRarely.g_OffsetVectors[0], sizeof(Vec4) * 14, &m_Offsets[0], sizeof(Vec4) * 14);

	m_tSSAO_ChangesRarely.g_BlurRadius = m_BlurRadius;
	memcpy_s(&m_tSSAO_ChangesRarely.g_BlurWeights[0], sizeof(float) * 11, &m_BlurWeights[0], sizeof(float) * 11);
	m_tSSAO_ChangesRarely.g_SurfaceEpsilon = m_SurfaceEpsilon;

	// CPU -> GPU [ register(b5) ] 
	CConstBuffer* pBuffer = CDevice::GetInst()->GetCB(CB_TYPE::SSAO_CHANGES_RARELY);
	pBuffer->SetData(&m_tSSAO_ChangesRarely, sizeof(tSSAO_ChangesRarely));
	pBuffer->UpdateData();
}

void CSSAO::UpdateData_SSAO()
{
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\SSAOmtrl.mtrl");

	Ptr<CTexture> pRandomVecTex   = CResMgr::GetInst()->FindRes<CTexture>(L"RandomVecTex");
	Ptr<CTexture> pNormalDepthTex = CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex");
	Ptr<CTexture> pAOTex          = CResMgr::GetInst()->FindRes<CTexture>(L"AmbientOcclusionTex");

	m_tViewPort          = {0.0f, 0.0f, (pAOTex->Width()), (pAOTex->Height())};
	m_tViewPort.MinDepth = 0.f;
	m_tViewPort.MaxDepth = 1.f;

	pMtrl->SetTexParam(TEX_PARAM::TEX_1, pNormalDepthTex);
	//pMtrl->SetTexParam(TEX_PARAM::TEX_2, pRandomVecTex);
	pMtrl->SetScalarParam(SCALAR_PARAM::INT_0, &m_SampleCount);


	pMtrl->UpdateData();
}

void CSSAO::RenderToSSAOTexture()
{
	UpdateData_SSAO();

	CONTEXT->IASetInputLayout(nullptr);
	CONTEXT->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	CONTEXT->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CONTEXT->RSSetViewports(1, &m_tViewPort);
	CONTEXT->Draw(3, 0);
}

void CSSAO::BlurAmbientMap()
{
	for (UINT i = 0; i < m_BlurCount; ++i)
	{
		BilateralBlurX();
		BilateralBlurY();
	}
}

void CSSAO::BilateralBlurX()
{
	Ptr<CTexture>  pAOTex = CResMgr::GetInst()->FindRes<CTexture>(L"AmbientOcclusionTex");
	Ptr<CMaterial> pMtrl  = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\SSAOBlurHorzmtrl.mtrl");
	pMtrl->UpdateData();

	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CONTEXT->RSSetViewports(1, &m_tViewPort);
	CONTEXT->OMSetRenderTargets(1, &pAOTex->GetRTV(), nullptr);

	CONTEXT->Draw(3, 0);
}

void CSSAO::BilateralBlurY()
{
	Ptr<CTexture>  pAOTex = CResMgr::GetInst()->FindRes<CTexture>(L"AmbientOcclusionTempTex");
	Ptr<CMaterial> pMtrl  = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\SSAOBlurVertmtrl.mtrl");
	pMtrl->UpdateData();

	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CONTEXT->RSSetViewports(1, &m_tViewPort);
	CONTEXT->OMSetRenderTargets(1, &pAOTex->GetRTV(), nullptr);

	CONTEXT->Draw(3, 0);
}

void CSSAO::BuildRandomVectorTexture()
{
	m_pRandomVectorTexture = CResMgr::GetInst()->CreateTexture(L"RandomVecTex",
	                                                           256,
	                                                           256,
	                                                           DXGI_FORMAT_R8G8B8A8_UNORM,
	                                                           D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	                                                           true);

	std::vector<XMCOLOR> randomVectors(256 * 256);

	// 난수 데이터 초기화
	std::mt19937 randEngine;
	randEngine.seed(std::random_device()());
	std::uniform_real_distribution<float> randF(0.0f, 1.0f);
	for (int i = 0; i < 256 * 256; ++i)
	{
		randomVectors[i] = XMCOLOR(randF(randEngine), randF(randEngine), randF(randEngine), 0.0f);
	}
	CONTEXT->UpdateSubresource(m_pRandomVectorTexture->GetTex2D().Get(),
	                           0,
	                           nullptr,
	                           randomVectors.data(),
	                           256 * sizeof(XMCOLOR),
	                           0);
}

void CSSAO::BuildOffsetVectors()
{
	// 14개의 균일한 간격의 벡터로 시작합니다. 입방체의 꼭지점 8개를 선택하고 입방체의 각 면을 따라 중심점을 잡습니다.
	// 우리는 항상 점을 반대쪽으로 번갈아 가며 만듭니다. 이 접근 방식은 14개 미만의 샘플링 포인트를 선택할 때 사용할 수 있습니다.
	// 여전히 벡터를 고르게 펼칠 수 있습니다.

	// 8 큐브 코너 벡터
	m_Offsets[0] = XMFLOAT4(+1.0f, +1.0f, +1.0f, 0.0f);
	m_Offsets[1] = XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.0f);

	m_Offsets[2] = XMFLOAT4(-1.0f, +1.0f, +1.0f, 0.0f);
	m_Offsets[3] = XMFLOAT4(+1.0f, -1.0f, -1.0f, 0.0f);

	m_Offsets[4] = XMFLOAT4(+1.0f, +1.0f, -1.0f, 0.0f);
	m_Offsets[5] = XMFLOAT4(-1.0f, -1.0f, +1.0f, 0.0f);

	m_Offsets[6] = XMFLOAT4(-1.0f, +1.0f, -1.0f, 0.0f);
	m_Offsets[7] = XMFLOAT4(+1.0f, -1.0f, +1.0f, 0.0f);

	// 6면 중심점 벡터
	m_Offsets[8] = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
	m_Offsets[9] = XMFLOAT4(+1.0f, 0.0f, 0.0f, 0.0f);

	m_Offsets[10] = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	m_Offsets[11] = XMFLOAT4(0.0f, +1.0f, 0.0f, 0.0f);

	m_Offsets[12] = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
	m_Offsets[13] = XMFLOAT4(0.0f, 0.0f, +1.0f, 0.0f);


	// 난수 데이터 초기화
	std::mt19937 randEngine;
	randEngine.seed(std::random_device()());
	std::uniform_real_distribution<float> randF(0.25f, 1.0f);
	for (int i = 0; i < 14; ++i)
	{
		// [0.25, 1.0] 범위에서 임의 길이의 벡터를 생성합니다.
		float s = randF(randEngine);

		XMVECTOR v = s * XMVector4Normalize(XMLoadFloat4(&m_Offsets[i]));

		XMStoreFloat4(&m_Offsets[i], v);
	}
}

void CSSAO::Begin() {}

void CSSAO::End() {}


//void SSAOManager::Begin(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* dsv, const D3D11_VIEWPORT& vp)
//{
//	// 지정된 DSV를 파이프라인에 바인딩합니다.
//	ID3D11RenderTargetView* pRTVs[1] = { m_pNormalDepthTexture->GetRenderTarget() };
//	deviceContext->OMSetRenderTargets(1, pRTVs, dsv);
//	deviceContext->RSSetViewports(1, &vp);
//
//	// 보기 공간 정상(0, 0, -1) 및 매우 큰 깊이 값으로 RTV를 지웁니다.
//	static const float clearColor[4] = { 0.0f, 0.0f, -1.0f, 1e5f };
//	deviceContext->ClearRenderTargetView(pRTVs[0], clearColor);
//	deviceContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//}
//
//void SSAOEffect::SetRenderNormalDepthMap(bool enableAlphaClip)
//{
//
//	pImpl->m_pCurrInputLayout = pImpl->m_pVertexPosNormalTexLayout.Get();
//	pImpl->m_pCurrEffectPass = pImpl->m_pEffectHelper->GetEffectPass("SSAO_Geometry");
//	pImpl->m_CurrTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//	pImpl->m_pCurrEffectPass->PSGetParamByName("alphaClip")->SetUInt(enableAlphaClip);
//
//}
//
//DrawScene<SSAOEffect>(m_SSAOEffect);
//
//void SSAOManager::End(ID3D11DeviceContext* deviceContext)
//{
//	// 解除RTV、DSV
//	deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
//}
//
//
//void SSAOManager::RenderToSSAOTexture(ID3D11DeviceContext* deviceContext, SSAOEffect& ssaoEffect, const Camera& camera)
//{
//	float zFar = camera.GetFarZ();
//	float halfHeight = zFar * tanf(0.5f * camera.GetFovY());
//	float halfWidth = camera.GetAspectRatio() * halfHeight;
//
//	// 단일 삼각형 렌더링, 제공된 먼 평면 포인트는 다음을 충족합니다.
//   // (-1, 1)--(3, 1)
//   //   |     /
//   //   |    /
//   //   |   /       
//   // (-1, -3)
//	XMFLOAT4 farPlanePoints[3] = {
//		XMFLOAT4(-halfWidth, halfHeight, zFar, 0.0f),
//		XMFLOAT4(3.0f * halfWidth, halfHeight, zFar, 0.0f),
//		XMFLOAT4(-halfWidth, -3.0f * halfHeight, zFar, 0.0f),
//	};
//	ssaoEffect.SetFrustumFarPlanePoints(farPlanePoints);
//	ssaoEffect.SetTextureRandomVec(m_pRandomVectorTexture->GetShaderResource());
//	ssaoEffect.SetOffsetVectors(m_Offsets);
//	ssaoEffect.SetProjMatrix(camera.GetProjMatrixXM());
//	ssaoEffect.SetOcclusionInfo(m_OcclusionRadius, m_OcclusionFadeStart, m_OcclusionFadeEnd, m_SurfaceEpsilon);
//	CD3D11_VIEWPORT vp(0.0f, 0.0f, (float)m_pAOTexture->GetWidth(), (float)m_pAOTexture->GetHeight());
//	ssaoEffect.RenderToSSAOTexture(deviceContext, m_pNormalDepthTexture->GetShaderResource(), m_pAOTexture->GetRenderTarget(), vp, m_SampleCount);
//}
//
//
//void SSAOManager::BlurAmbientMap(ID3D11DeviceContext* deviceContext, SSAOEffect& ssaoEffect)
//{
//	CD3D11_VIEWPORT vp(0.0f, 0.0f, (float)m_pAOTempTexture->GetWidth(), (float)m_pAOTempTexture->GetHeight());
//	ssaoEffect.SetBlurRadius(m_BlurRadius);
//	ssaoEffect.SetBlurWeights(m_BlurWeights);
//	// 각 블러에는 수평 블러와 수직 블러가 필요합니다.
//	for (uint32_t i = 0; i < m_BlurCount; ++i)
//	{
//		ssaoEffect.BilateralBlurX(deviceContext, m_pAOTexture->GetShaderResource(), m_pNormalDepthTexture->GetShaderResource(), m_pAOTempTexture->GetRenderTarget(), vp);
//		ssaoEffect.BilateralBlurY(deviceContext, m_pAOTempTexture->GetShaderResource(), m_pNormalDepthTexture->GetShaderResource(), m_pAOTexture->GetRenderTarget(), vp);
//	}
//}
