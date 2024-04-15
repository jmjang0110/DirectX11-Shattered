#pragma once
#include "CRenderComponent.h"

#include "CRaycastShader.h"
#include "CHeightMapShader.h"
#include "CWeightMapShader.h"

class CStructuredBuffer;

class CLandScape : public CRenderComponent
{
private:
	Ptr<CTexture> m_pHeightMap;

	UINT m_iXFaceCount;
	UINT m_iZFaceCount;

	Ptr<CRaycastShader> m_pCSRaycast;   // 픽킹 쉐이더
	CStructuredBuffer*  m_pCrossBuffer;	// 마우스 피킹되는 지점 정보 받는 버퍼

	Ptr<CHeightMapShader> m_pCSHeightMap; // 높이맵 쉐이더
	Ptr<CTexture>         m_pBrushArrTex; // 브러쉬용 텍스쳐

	Ptr<CWeightMapShader> m_pCSWeightMap;     // 가중치 쉐이더
	CStructuredBuffer*    m_pWeightMapBuffer; // 가중치 저장 버퍼
	UINT                  m_iWeightWidth;		// 가중치 버퍼 가로세로 행렬 수
	UINT                  m_iWeightHeight;	// 가중치 버퍼 가로세로 행렬 수
	UINT                  m_iWeightIdx;		// 증가 시킬 가중치 부위

	Ptr<CTexture> m_pTileArrTex;  // 타일 배열 텍스쳐

	Vec2 m_vBrushScale;
	int  m_iBrushIdx;    // 브러쉬 인덱스

	LANDSCAPE_MOD m_eMod; 	// 지형 툴모드에서 상태값

public:
	void finalupdate() override;
	void render() override;

public:
	void SetFaceCount(UINT _iX, UINT _iZ)
	{
		m_iXFaceCount = _iX;
		m_iZFaceCount = _iZ;
	}

	void Create();

private:
	void Raycasting();

private:
	void CreateShaderMaterial();
	void CreateMesh();
	void CreateTexture();

public:
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(CLandScape);
	CLandScape();
	virtual ~CLandScape();
};
