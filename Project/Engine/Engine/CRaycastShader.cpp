#include "pch.h"
#include "CRaycastShader.h"

#include "CStructuredBuffer.h"
#include "CTexture.h"

CRaycastShader::CRaycastShader()
	: CComputeShader(32, 32, 1)
	, m_iXFace(0)
	, m_iZFace(0)
	, m_ray{}
	, m_pOutput(nullptr) {}

CRaycastShader::~CRaycastShader() = default;

void CRaycastShader::UpdateData()
{
	m_Param.iArr[0] = m_iXFace;
	m_Param.iArr[1] = m_iZFace;

	m_Param.v4Arr[0] = m_ray.vStart;
	m_Param.v4Arr[1] = m_ray.vDir;

	// 높이맵 (SRV)
	m_pHeightMap->UpdateData_CS(0, true);

	// 출력 버퍼	(UAV)
	m_pOutput->UpdateData_CS(0, false);

	// 그룹 수 계산
	m_iGroupX = ((m_iXFace * 2) / m_iGroupPerThreadCountX) + 1;
	m_iGroupY = (m_iZFace / m_iGroupPerThreadCountY) + 1;
	m_iGroupZ = 1;
}

void CRaycastShader::Clear()
{
	m_pHeightMap->ClearCS(0);
	m_pOutput->Clear();
}
