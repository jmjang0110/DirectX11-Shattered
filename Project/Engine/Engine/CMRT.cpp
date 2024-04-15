#include "pch.h"
#include "CMRT.h"

#include "CDevice.h"

CMRT::CMRT()
	: m_arrRT{}
	, m_iRTCount(0)
	, m_pDSTex(nullptr)
	, m_arrClearColor{
		Vec4(0.f, 0.f, 0.f, 1.f),
		Vec4(0.f, 0.f, 0.f, 1.f),
		Vec4(0.f, 0.f, 0.f, 1.f),
		Vec4(0.f, 0.f, 0.f, 1.f),
		Vec4(0.f, 0.f, 0.f, 1.f),
		Vec4(0.f, 0.f, 0.f, 1.f),
		Vec4(0.f, 0.f, 0.f, 1.f),
		Vec4(0.f, 0.f, 0.f, 1.f)
	}
	, m_tViewPort{} {}

CMRT::~CMRT() {}

void CMRT::Create(int _iRenderTargetCount, Ptr<CTexture>* _ppTex, Ptr<CTexture> _pDSTex)
{
	m_iRTCount = _iRenderTargetCount;

	for (int i = 0; i < m_iRTCount; ++i)
	{
		m_arrRT[i] = _ppTex[i];
	}

	m_pDSTex = _pDSTex;

	// ViewPort 설정	
	m_tViewPort.TopLeftX = 0;
	m_tViewPort.TopLeftY = 0;
	m_tViewPort.Width    = m_arrRT[0]->Width();
	m_tViewPort.Height   = m_arrRT[0]->Height();
	m_tViewPort.MinDepth = 0;
	m_tViewPort.MaxDepth = 1;
}

void CMRT::SetClearColor(int _iCount, const Vec4* _pColor)
{
	for (int i = 0; i < _iCount; ++i)
	{
		m_arrClearColor[i] = _pColor[i];
	}
}

void CMRT::OMSet()
{
	ID3D11RenderTargetView* arrView[8] = {};

	for (int i = 0; i < m_iRTCount; ++i)
	{
		arrView[i] = m_arrRT[i]->GetRTV().Get();
	}

	if (nullptr != m_pDSTex)
	{
		CONTEXT->OMSetRenderTargets(m_iRTCount, arrView, m_pDSTex->GetDSV().Get());
	}

	else
	{
		// MRT 에 DSTex 가 없는 경우, 장치에 있던걸 그대로 쓴다.

		//ID3D11DepthStencilView* pDSView = nullptr;
		//CONTEXT->OMGetRenderTargets(0, nullptr, &pDSView);
		//pDSView->Release();

		ComPtr<ID3D11DepthStencilView> pDSView = nullptr;
		CONTEXT->OMGetRenderTargets(0, nullptr, pDSView.GetAddressOf());
		CONTEXT->OMSetRenderTargets(m_iRTCount, arrView, pDSView.Get());
	}

	// ViewPort 설정
	CONTEXT->RSSetViewports(1, &m_tViewPort);
}

void CMRT::Clear()
{
	for (int i = 0; i < m_iRTCount; ++i)
	{
		CONTEXT->ClearRenderTargetView(m_arrRT[i]->GetRTV().Get(), m_arrClearColor[i]);
	}

	if (nullptr != m_pDSTex)
		CONTEXT->ClearDepthStencilView(m_pDSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}
