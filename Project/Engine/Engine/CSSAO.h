#pragma once
#include "CTexture.h"
#include "singleton.h"

class CSSAO
	: public CSingleton<CSSAO>
{
	SINGLE(CSSAO);

private:
	// SSAO 기본 설정, 수정 가능
	uint32_t m_SampleCount        = 14;           // 샘플 벡터의 수
	uint32_t m_BlurCount          = 4;            // 블러 수
	float    m_OcclusionRadius    = 2.f;          // 반구의 반경 샘플링
	float    m_OcclusionFadeStart = 0.2f;         // 폐색되지 않은 전환을 시작하기 위한 시작 거리 값
	float    m_OcclusionFadeEnd   = 2.0f;         // 완전히 차단되지 않은 거리 값
	float    m_SurfaceEpsilon     = 0.05f;        // 자기 교차를 방지하기 위한 거리 값
	float    m_BlurWeights[11]    = {0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f};
	uint32_t m_BlurRadius         = 5;

	Vec4 m_Offsets[14] = {};

private:
	// UpdateData SSAO Implement 
	tSSAO_ChangesOnResize m_tSSAO_ChangesOnResize;
	tSSAO_ChangesRarely   m_tSSAO_ChangesRarely;

	D3D11_VIEWPORT m_tViewPort;

private:
	Ptr<CTexture> m_pNormalDepthTexture;           // 일반/깊이 텍스처
	Ptr<CTexture> m_pAOTexture;                    // 앰비언트 오클루전 맵
	Ptr<CTexture> m_pAOTempTexture;                // 중간 앰비언트 오클루전 맵
	Ptr<CTexture> m_pRandomVectorTexture;          // 랜덤 벡터 텍스처

public:
	void Init();
	void Apply();


	void Init_SSAO_Texture();
	void Init_SSAO_NormalDepthMtrl(); // SSAO_Geometry
	void Init_SSAO_Mtrl();
	void Init_SSAO_BlurHorzMtrl();
	void Init_SSAO_BlurVertMtrl();
	void Init_SSAO_DebugMtrl();

	void UpdateData_SSAO_ChangesOnResize();
	void UpdateData_SSAO_ChangesRarely();

	void UpdateData_SSAO();

public:
	void RenderToSSAOTexture();
	void BlurAmbientMap();
	void BilateralBlurX();
	void BilateralBlurY();

public:
	void BuildRandomVectorTexture();
	void BuildOffsetVectors();

public:
	void Begin();
	void End();

public:
	void SetSampleCount(uint32_t count) { m_SampleCount = count; }
};
