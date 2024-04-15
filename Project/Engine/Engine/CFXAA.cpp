#include "pch.h"
#include "CFXAA.h"

#include "CGraphicsShader.h"
#include "CKeyMgr.h"
#include "CDevice.h"


CFXAA::CFXAA()
	: m_tViewPort{} {}

CFXAA::~CFXAA() = default;

void CFXAA::SetViewPort(D3D11_VIEWPORT _tViewPort)
{
	m_tViewPort = _tViewPort;
}

void CFXAA::Init()
{
	m_tViewPort.MinDepth = 0.f;
	m_tViewPort.MaxDepth = 1.f;

	InitShader();
	InitMaterial();
}

void CFXAA::InitShader()
{
	// SHADER 
	CGraphicsShader* pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\FullScreenTriangle.fx", "VS_FullScreenTriangleTexcoord");
	pShader->CreatePixelShader(L"shader\\FXAA.fx", "PS_FXAA");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::NO_ALPHA_COVERAGE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CResMgr::GetInst()->AddRes<CGraphicsShader>(L"FXAAShader", pShader, true);
}

void CFXAA::InitMaterial()
{
	const Ptr<CTexture> pPostTex         = CResMgr::GetInst()->FindRes<CTexture>(L"PostProcessTex");
	m_sImplement.QualitySubPix           = 1.f;
	m_sImplement.QualityEdgeThreshold    = 0.166f;
	m_sImplement.QualityEdgeThresholdMin = 0.0833f;
	m_sImplement.EnableDebug             = false;
	m_sImplement.TexelSize               = Vec2(1.0f / m_tViewPort.Width, 1.0f / m_tViewPort.Height);

	// MATERIAL
	CMaterial* pMtrl = new CMaterial;
	pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"FXAAShader"));
	pMtrl->SetTexParam(TEX_PARAM::TEX_0, pPostTex);
	pMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_0, &m_sImplement.QualitySubPix);
	pMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_1, &m_sImplement.QualityEdgeThreshold);
	pMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_2, &m_sImplement.QualityEdgeThresholdMin);
	pMtrl->SetScalarParam(SCALAR_PARAM::INT_0, &m_sImplement.EnableDebug);
	pMtrl->SetScalarParam(SCALAR_PARAM::VEC2_0, &m_sImplement.TexelSize);
	CResMgr::GetInst()->AddRes<CMaterial>(L"material\\FXAAmtrl.mtrl", pMtrl);
	m_pMtrl = pMtrl;
}


void CFXAA::SetQuality(int major, int minor)
{
	m_sImplement.Major = major;
	m_sImplement.Minor = minor;
}

void CFXAA::SetQualitySubPix(float val)
{
	m_sImplement.QualitySubPix = val;
}

void CFXAA::SetQualityEdgeThreshold(float threshold)
{
	m_sImplement.QualityEdgeThreshold = threshold;
}

void CFXAA::SetQualityEdgeThresholdMin(float thresholdMin)
{
	m_sImplement.QualityEdgeThresholdMin = thresholdMin;
}

void CFXAA::EnableDebug(bool enabled)
{
	m_sImplement.EnableDebug = enabled;
}

void CFXAA::UpdateData()
{
	if (KEY_TAP(KEY::J))
	{
		EnableDebug(!m_sImplement.EnableDebug);
	}
	m_sImplement.TexelSize = Vec2(1.0f / m_tViewPort.Width, 1.0f / m_tViewPort.Height);

	m_pMtrl->SetScalarParam(SCALAR_PARAM::INT_0, &m_sImplement.EnableDebug);
	m_pMtrl->SetScalarParam(SCALAR_PARAM::VEC2_0, &m_sImplement.TexelSize);
	m_pMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_0, &m_sImplement.QualitySubPix);
	m_pMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_1, &m_sImplement.QualityEdgeThreshold);
	m_pMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_2, &m_sImplement.QualityEdgeThresholdMin);

	m_pMtrl->UpdateData();
}

void CFXAA::Apply()
{
	UpdateData();

	CONTEXT->IASetInputLayout(nullptr);
	CONTEXT->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	CONTEXT->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);

	CONTEXT->RSSetViewports(1, &m_tViewPort);
	CONTEXT->Draw(3, 0);
}
