#include "pch.h"
#include "CLandScape.h"

#include "CRenderMgr.h"
#include "CKeyMgr.h"
#include "CTransform.h"
#include "CCamera.h"
#include "CStructuredBuffer.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_iXFaceCount(0)
	, m_iZFaceCount(0)
	, m_pCrossBuffer(nullptr)
	, m_pWeightMapBuffer(nullptr)
	, m_iWeightWidth(0)
	, m_iWeightHeight(0)
	, m_iWeightIdx(0)
	, m_vBrushScale(1.f, 1.f)
	, m_iBrushIdx(0)
	, m_eMod(LANDSCAPE_MOD::NONE)
{
	m_pCrossBuffer = new CStructuredBuffer;
	m_pCrossBuffer->Create(sizeof(tRaycastOut), 1, SB_TYPE::READ_WRITE, true, nullptr);
}

CLandScape::~CLandScape()
{
	SAFE_DELETE(m_pCrossBuffer);
	SAFE_DELETE(m_pWeightMapBuffer);
}

void CLandScape::finalupdate()
{
	if (KEY_TAP(KEY::NUM0))
		m_eMod = LANDSCAPE_MOD::NONE;
	else if (KEY_TAP(KEY::NUM1))
		m_eMod = LANDSCAPE_MOD::HEIGHT_MAP;
	else if (KEY_TAP(KEY::NUM2))
		m_eMod = LANDSCAPE_MOD::SPLAT;

	if (LANDSCAPE_MOD::NONE == m_eMod)
	{
		return;
	}

	Raycasting();

	// 픽킹 정보를 system memory 로 가져옴
	//m_pCrossBuffer->GetData(&out);

	if (KEY_PRESSED(KEY::LBTN))
	{
		m_vBrushScale = Vec2(0.1f, 0.1f);

		if (LANDSCAPE_MOD::HEIGHT_MAP == m_eMod)
		{
			// 교점 위치정보를 토대로 높이를 수정 함
			m_pCSHeightMap->SetInputBuffer(m_pCrossBuffer); // 픽킹 정보를 HeightMapShader 에 세팅
			m_pCSHeightMap->SetBrushTex(m_pBrushArrTex);	// 사용할 브러쉬 텍스쳐 세팅
			m_pCSHeightMap->SetBrushIndex(m_iBrushIdx);		// 브러쉬 인덱스 설정
			m_pCSHeightMap->SetBrushScale(m_vBrushScale);   // 브러쉬 크기
			m_pCSHeightMap->SetHeightMap(m_pHeightMap);
			m_pCSHeightMap->Excute();
		}
		else if (LANDSCAPE_MOD::SPLAT == m_eMod)
		{
			// 교점 위치정보를 가중치를 수정함	
			m_pCSWeightMap->SetBrushArrTex(m_pBrushArrTex);
			m_pCSWeightMap->SetBrushIndex(m_iBrushIdx);
			m_pCSWeightMap->SetBrushScale(m_vBrushScale); // 브러쉬 크기
			m_pCSWeightMap->SetWeightMap(m_pWeightMapBuffer, m_iWeightWidth, m_iWeightHeight); // 가중치맵, 가로 세로 개수
			m_pCSWeightMap->SetInputBuffer(m_pCrossBuffer); // 레이 캐스트 위치
			m_pCSWeightMap->SetWeightIdx(m_iWeightIdx);
			m_pCSWeightMap->Excute();
		}
	}
}

void CLandScape::render()
{
	Transform()->UpdateData();

	// 가중치 버퍼 전달
	m_pWeightMapBuffer->UpdateData(PS, 17);

	// 가중치 버퍼 해상도 전달
	auto vWeightMapResolution = Vec2(static_cast<float>(m_iWeightWidth), static_cast<float>(m_iWeightHeight));
	GetMaterial(0)->SetScalarParam(SCALAR_PARAM::VEC2_1, &vWeightMapResolution);

	// 타일 배열 개수 전달
	float m_fTileCount = static_cast<float>(m_pTileArrTex->GetArraySize() / 2); // 색상, 노말 합쳐져있어서 나누기 2 해줌
	GetMaterial(0)->SetScalarParam(SCALAR_PARAM::FLOAT_0, &m_fTileCount);

	// 타일 텍스쳐 전달
	GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_ARR_0, m_pTileArrTex);

	GetMaterial(0)->UpdateData();
	GetMesh()->render(0);

	m_pWeightMapBuffer->Clear();
}


void CLandScape::Raycasting()
{
	// 시점 카메라를 가져옴
	const CCamera* pMainCam = CRenderMgr::GetInst()->GetMainCam();
	if (nullptr == pMainCam)
	{
		return;
	}

	// 월드 기준 광선을 지형의 로컬로 보냄
	const Matrix& matWorldInv = Transform()->GetWorldInvMat();
	const tRay&   ray         = pMainCam->GetRay();

	tRay CamRay   = {};
	CamRay.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
	CamRay.vDir   = XMVector3TransformNormal(ray.vDir, matWorldInv);
	CamRay.vDir.Normalize();

	// 지형과 카메라 Ray 의 교점을 구함
	const tRaycastOut out = {Vec2(0.f, 0.f), 0x7fffffff, 0};
	m_pCrossBuffer->SetData(&out, 1);

	m_pCSRaycast->SetHeightMap(m_pHeightMap);
	m_pCSRaycast->SetFaceCount(m_iXFaceCount, m_iZFaceCount);
	m_pCSRaycast->SetCameraRay(CamRay);
	m_pCSRaycast->SetOuputBuffer(m_pCrossBuffer);

	m_pCSRaycast->Excute();
}
