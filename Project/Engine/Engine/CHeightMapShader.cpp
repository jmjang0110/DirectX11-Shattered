#include "pch.h"
#include "CHeightMapShader.h"

#include "CStructuredBuffer.h"
#include "CTexture.h"

CHeightMapShader::CHeightMapShader()
	: CComputeShader(32, 32, 1)
	, m_iBrushIdx(0)
	, m_pInput(nullptr) {}

CHeightMapShader::~CHeightMapShader() = default;

void CHeightMapShader::UpdateData()
{
	m_Param.iArr[0]  = static_cast<int>(m_pHeightMap->Width());
	m_Param.iArr[1]  = static_cast<int>(m_pHeightMap->Height());
	m_Param.iArr[2]  = m_iBrushIdx;
	m_Param.v2Arr[0] = m_vScale;

	// 높이맵
	m_pHeightMap->UpdateData_CS(0, false);
	// 브러쉬
	m_pBrushTex->UpdateData_CS(0, true);
	// 피킹정보
	m_pInput->UpdateData_CS(16, true);

	// 실행 시킬 스레드 그룹 수 지정
	m_iGroupX = (static_cast<UINT>(m_pHeightMap->Width()) / m_iGroupPerThreadCountX) + 1;
	m_iGroupY = (static_cast<UINT>(m_pHeightMap->Height()) / m_iGroupPerThreadCountY) + 1;
	m_iGroupZ = 1;
}

void CHeightMapShader::Clear()
{
	if (nullptr != m_pHeightMap)
	{
		m_pHeightMap->ClearCS(0);
		m_pHeightMap = nullptr;
	}

	if (nullptr != m_pBrushTex)
	{
		m_pBrushTex->Clear(0);
		m_pBrushTex = nullptr;
	}

	if (nullptr != m_pInput)
	{
		m_pInput->Clear();
		m_pInput = nullptr;
	}
}
