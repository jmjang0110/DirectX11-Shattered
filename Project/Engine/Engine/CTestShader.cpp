#include "pch.h"
#include "CTestShader.h"

CTestShader::CTestShader()
	: CComputeShader(32, 32, 1) {}

CTestShader::~CTestShader() = default;

void CTestShader::UpdateData()
{
	m_pOutputTex->UpdateData_CS(0, false);

	// 호출 그룹 개수 계산
	const UINT remainForWidth  = (UINT)m_pOutputTex->Width() % m_iGroupPerThreadCountX;
	m_iGroupX                  = (UINT)m_pOutputTex->Width() / m_iGroupPerThreadCountX + (remainForWidth > 0) ? 1 : 0;
	const UINT remainForHeight = (UINT)m_pOutputTex->Height() % m_iGroupPerThreadCountY;
	m_iGroupY                  = (UINT)m_pOutputTex->Height() / m_iGroupPerThreadCountY + (remainForHeight > 0) ? 1 : 0;
	m_iGroupZ                  = 1;
}

void CTestShader::Clear()
{
	CTexture::ClearCS(0);
}
