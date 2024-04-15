#include "pch.h"
#include "CAnimation3DShader.h"

#include "CStructuredBuffer.h"

CAnimation3DShader::CAnimation3DShader()
	: CComputeShader(256, 1, 1)
	, m_pFrameDataBuffer(nullptr)
	, m_pOffsetMatBuffer(nullptr)
	, m_pOutputBuffer(nullptr) {}

CAnimation3DShader::~CAnimation3DShader() = default;

void CAnimation3DShader::UpdateData()
{
	// ����ȭ���� ����
	m_pFrameDataBuffer->UpdateData_CS(16, true); // t16
	m_pOffsetMatBuffer->UpdateData_CS(17, true); // t17
	m_pOutputBuffer->UpdateData_CS(0, false);   // u0

	//�������� 1������ UAV ������Ʈ(�׷��Ƚ� ���������ο����� ������� �����Ƿ�, false)
	m_pSocketBuffer->UpdateData_CS(1, false);

	m_iGroupX = (m_Param.iArr[0] / m_iGroupPerThreadCountX) + 1;
	m_iGroupY = 1;
	m_iGroupZ = 1;
}

void CAnimation3DShader::Clear()
{
	// ������ ����ȭ���� Ŭ����	
	if (nullptr != m_pFrameDataBuffer)
	{
		m_pFrameDataBuffer->Clear();
		m_pFrameDataBuffer = nullptr;
	}

	if (nullptr != m_pOffsetMatBuffer)
	{
		m_pOffsetMatBuffer->Clear();
		m_pOffsetMatBuffer = nullptr;
	}

	if (nullptr != m_pOutputBuffer)
	{
		m_pOutputBuffer->Clear();
		m_pOutputBuffer = nullptr;
	}

	if (nullptr != m_pSocketBuffer)
	{
		m_pSocketBuffer->Clear();
		m_pSocketBuffer = nullptr;
	}
}