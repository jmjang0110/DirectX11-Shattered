#include "pch.h"
#include "CRenderEffectMgr.h"
#include "CRenderMgr.h"
#include "CFXAA.h"
#include "CSSAO.h"

#include "CMRT.h"
#include "CTimeMgr.h"
#include "CDevice.h"


CRenderEffectMgr::CRenderEffectMgr()
	: m_bEnable_SSAO(true)
	, m_bEnable_FXAA(true)
	, m_bEnable_FadeIn_PaperBurn{false}
	, m_bEnable_FadeOut_PaperBurn{false}
	, m_fPaperBurn_Timer(0.f)
	, m_fDuration{0}
{
}

CRenderEffectMgr::~CRenderEffectMgr()
{
}

void CRenderEffectMgr::Init()
{
	CFXAA::GetInst()->Init();
	CSSAO::GetInst()->Init();
}

void CRenderEffectMgr::Apply(EFFECT_TYPE _eType)
{
	switch (_eType)
	{
	case EFFECT_TYPE::FXAA:
		{
			if (!m_bEnable_FXAA)
				break;

			CRenderMgr::GetInst()->CopyTargetToPostProcess();
			const D3D11_VIEWPORT SwapChain_VP = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->GetViewPort();
			CFXAA::GetInst()->SetViewPort(SwapChain_VP);
			CFXAA::GetInst()->Apply();
		}
		break;
	case EFFECT_TYPE::SSAO:
		{
			if (!m_bEnable_SSAO)
				break;

			CSSAO::GetInst()->Apply();
		}
		break;

	case EFFECT_TYPE::SSAO_BLUR:
		{
			if (!m_bEnable_SSAO)
				break;

			CSSAO::GetInst()->BlurAmbientMap();
		}
		break;
	case EFFECT_TYPE::FADE_IN_PAPERBURN:
		{
			if (!m_bEnable_FadeIn_PaperBurn || m_bFadeInOutFinished)
				break;

			m_fPaperBurn_Timer += DT / m_fDuration;
			if (m_fPaperBurn_Timer >= 1.f)
			{
				m_fPaperBurn_Timer   = 1.f;
				m_bFadeInOutFinished = true;
			}

			CRenderMgr::GetInst()->CopyTargetToPostProcess();
			Apply_FadeInOut_PaperBurn();
		}
		break;
	case EFFECT_TYPE::FADE_OUT_PAPERBURN:
		{
			if (!m_bEnable_FadeOut_PaperBurn || m_bFadeInOutFinished)
				break;

			m_fPaperBurn_Timer -= DT / m_fDuration;
			if (m_fPaperBurn_Timer <= 0.f)
			{
				m_fPaperBurn_Timer   = 0.f;
				m_bFadeInOutFinished = true;
			}

			CRenderMgr::GetInst()->CopyTargetToPostProcess();
			Apply_FadeInOut_PaperBurn();
		}
		break;
	}
}

void CRenderEffectMgr::Init_FadePaperBurn()
{
}

void CRenderEffectMgr::Apply_FadeInOut_PaperBurn()
{
	const D3D11_VIEWPORT SwapChain_VP = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->GetViewPort();
	Ptr<CMesh> pRectMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	Ptr<CMaterial> pPaperBurnMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"material\\PaperBurnFullScreenMtrl.mtrl");
	const Ptr<CTexture> pPostTex = CResMgr::GetInst()->FindRes<CTexture>(L"PostProcessTex");

	const Ptr<CTexture> pNoiseTex
		= CResMgr::GetInst()->FindRes<CTexture>(L"texturetexture\\UI\\FX\\UI_StylizedClouds_FX.png");
	const Ptr<CTexture> pResultTex
		= CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\StartMenu\\StartMenu_UI_BG.png");

	pPaperBurnMtrl->SetTexParam(TEX_PARAM::TEX_0, pPostTex);
	pPaperBurnMtrl->SetTexParam(TEX_PARAM::TEX_1, pNoiseTex);
	pPaperBurnMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_0, &m_fPaperBurn_Timer);
	pPaperBurnMtrl->SetScalarParam(SCALAR_PARAM::VEC4_0, &m_color);
	if (m_bBlack)
	{
		pPaperBurnMtrl->SetScalarParam(SCALAR_PARAM::INT_0, &m_bBlack);
		pPaperBurnMtrl->SetTexParam(TEX_PARAM::TEX_2, nullptr);
	}
	else
	{
		pPaperBurnMtrl->SetTexParam(TEX_PARAM::TEX_2, pResultTex);
	}
	pPaperBurnMtrl->UpdateData();

	CONTEXT->IASetInputLayout(nullptr);
	CONTEXT->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	CONTEXT->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);

	CONTEXT->RSSetViewports(1, &SwapChain_VP);
	CONTEXT->Draw(3, 0);
}
