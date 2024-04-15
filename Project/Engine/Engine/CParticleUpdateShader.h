#pragma once
#include "CComputeShader.h"

class CParticleUpdateShader : public CComputeShader
{
private:
	CStructuredBuffer* m_ParticleBuffer;
	CStructuredBuffer* m_ParticleDataBuffer;

public:
	void SetParticleBuffer(CStructuredBuffer* _buffer) { m_ParticleBuffer = _buffer; }
	void SetParticleDataBuffer(CStructuredBuffer* _buffer) { m_ParticleDataBuffer = _buffer; }

	void SetObjectWorldPos(const Vec3& _vWorldPos)
	{
		m_Param.fArr[1] = _vWorldPos.x;
		m_Param.fArr[2] = _vWorldPos.y;
		m_Param.fArr[3] = _vWorldPos.z;
	}

	void SetParticleCreateDistance(float _fDist) { m_Param.fArr[0] = _fDist; }
	void SetParticleMinMaxTime(float _fMin, float _fMax) { m_Param.v2Arr[0] = Vec2(_fMin, _fMax); }
	void SetStartEndSpeed(float _fMin, float _fMax) { m_Param.v2Arr[1] = Vec2(_fMin, _fMax); }
	void SetDirection(Vec2 _vDirection) { m_Param.v2Arr[2] = _vDirection; }
	void SetAngle(float _angle) { m_Param.v2Arr[3] = Vec2(_angle, 0.f); }

	void SetSpeedDetailData(int _funcNum);

	void SetStartEndColor(const Vec4& _vStart, const Vec4& _vEnd)
	{
		m_Param.v4Arr[0] = _vStart;
		m_Param.v4Arr[1] = _vEnd;
	}

	void SetStartEndScale(const Vec3& _vStart, const Vec3& _vEnd)
	{
		m_Param.v4Arr[2] = _vStart;
		m_Param.v4Arr[3] = _vEnd;
	}

	void SetStartEndEmissiveColor(const Vec4& _vStart, const Vec4& _vEnd)
	{
		// 나중에... 다른 타겟에..
		//m_Param.v4_Emissive_Arr[0] = _vStart;
		//m_Param.v4_Emissive_Arr[1] = _vEnd;
	}

public:
	void UpdateData() override;
	void Clear() override;

public:
	CParticleUpdateShader();
	virtual ~CParticleUpdateShader();
};
