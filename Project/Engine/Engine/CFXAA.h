#pragma once
#include "singleton.h"

/*
    * FXAA : Fast Approximate Anti-Aliasing
            - Post Processing 
*/

// QualitySubPix
// 선명도에 영향
// 1.00 - 부드러운
// 0.75 - 기본 필터 값
// 0.50 - 더 선명하고 하위 픽셀 앨리어싱을 덜 제거합니다.
// 0.25 - 살짝 꺼짐 
// 0.00 - 완전히 꺼짐   

// QualityEdgeThreshold
// 원하는 로컬 콘트라스트의 임계값 제어
// 0.333 - 매우 낮음(더 빠름)
// 0.250 - 저품질
// 0.166 - 기본
// 0.125 - 고품질
// 0.063 - 매우 높음(느림)


// QualityEdgeThresholdMin
// 어두운 영역을 처리하지 않는 임계값
// 0.0833 - 기본
// 0.0625 - 약간 더 빠르게
// 0.0312 - 느리게

struct FXAA_Implement
{
	int Major       = 3;
	int Minor       = 9;
	int EnableDebug = 0;

	float QualitySubPix           = 0.75f;
	float QualityEdgeThreshold    = 0.166f;
	float QualityEdgeThresholdMin = 0.0833f;

	Vec2 TexelSize;
};


class CFXAA : public CSingleton<CFXAA>

{
	SINGLE(CFXAA);

private:
	FXAA_Implement m_sImplement;
	D3D11_VIEWPORT m_tViewPort;

	Ptr<CMaterial> m_pMtrl;

public:
	void SetViewPort(D3D11_VIEWPORT _tViewPort);

	void Init();
	void UpdateData();
	void Apply();

	void InitShader();
	void InitMaterial();

	// major = 1 저품질,   minor = 0...5
	// major = 2 중품질    minor = 0...9
	// major = 3 고품질,   minor = 9
	void SetQuality(int major, int minor);
	void SetQualitySubPix(float val);
	void SetQualityEdgeThreshold(float threshold);
	void SetQualityEdgeThresholdMin(float thresholdMin);
	void EnableDebug(bool enabled);
};
