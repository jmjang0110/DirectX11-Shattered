#include "pch.h"
#include "CParticleUpdateShader.h"
#include "CStructuredBuffer.h"

CParticleUpdateShader::CParticleUpdateShader()
	: CComputeShader(32, 1, 1)
	, m_ParticleBuffer(nullptr)
	, m_ParticleDataBuffer{nullptr} {}

CParticleUpdateShader::~CParticleUpdateShader() = default;

void CParticleUpdateShader::SetSpeedDetailData(int _funcNum)
{
	m_Param.iArr[3] = _funcNum;
}

void CParticleUpdateShader::UpdateData()
{
	m_ParticleBuffer->UpdateData_CS(0, false);
	m_ParticleDataBuffer->UpdateData_CS(1, false);

	//나머지가 존재한다면 스레드 그룹 개수를 1개 더 늘린다.
	const UINT remainCount = m_ParticleBuffer->GetElementCount() % m_iGroupPerThreadCountX;
	m_iGroupX              = m_ParticleBuffer->GetElementCount() / m_iGroupPerThreadCountX + (remainCount > 0) ? 1 : 0;
	m_iGroupY              = 1;
	m_iGroupZ              = 1;

	// Max Thread Count
	m_Param.iArr[0] = m_iGroupX * m_iGroupPerThreadCountX;
	m_Param.iArr[1] = m_ParticleBuffer->GetElementCount();
}

void CParticleUpdateShader::Clear()
{
	m_ParticleBuffer->Clear();
}
