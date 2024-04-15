#include "pch.h"
#include "CResMgr.h"

# include "CTestShader.h"
#include "CParticleUpdateShader.h"
#include "CAnimation3DShader.h"
#include "CThreadPool.h"

namespace
{
	void MakeGameShader()
	{
		CGraphicsShader* pShader{};
		//SINE-WAVE POST-PROCESS
		pShader = new CGraphicsShader;
		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
		pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_PostProcess");
		pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_SinePostProcess");
		pShader->SetDSType(DS_TYPE::NO_WRITE);
		pShader->SetRSType(RS_TYPE::CULL_NONE);
		CResMgr::GetInst()->AddRes<CGraphicsShader>(L"SineWavePostProcessShader", pShader, true);

		//Absorb POST-PROCESS 
		pShader = new CGraphicsShader;
		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_FORWARD);
		pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_PostProcess");
		pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_AbsorbPostProcess");
		pShader->SetDSType(DS_TYPE::NO_WRITE);
		pShader->SetRSType(RS_TYPE::CULL_NONE);
		CResMgr::GetInst()->AddRes<CGraphicsShader>(L"AbsorbPostProcessShader", pShader, true);

		//UI Shader
		pShader = new CGraphicsShader;
		pShader->CreateVertexShader(L"shader\\ui.fx", "VS_UI");
		pShader->CreatePixelShader(L"shader\\ui.fx", "PS_UI");

		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_FORWARD);
		pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
		pShader->SetRSType(RS_TYPE::CULL_BACK);

		pShader->AddTexParamInfo(L"UI TEXTURE", TEX_PARAM::TEX_0);
		pShader->AddScalarParamInfo(L"UI TYPE", SCALAR_PARAM::INT_0);
		pShader->AddScalarParamInfo(L"USE INFO", SCALAR_PARAM::INT_1);
		pShader->AddScalarParamInfo(L"USE PROGRESS BAR", SCALAR_PARAM::INT_2);
		pShader->AddScalarParamInfo(L"PROGRESS BAR DIRECTION", SCALAR_PARAM::INT_3);
		pShader->AddScalarParamInfo(L"PERCENTAGE", SCALAR_PARAM::FLOAT_0);
		pShader->AddScalarParamInfo(L"TEXTURE SIZE", SCALAR_PARAM::VEC2_0);
		pShader->AddScalarParamInfo(L"UI INFO", SCALAR_PARAM::VEC4_0);

		CResMgr::GetInst()->AddRes<CGraphicsShader>(L"UIShader", pShader, true);

		//UI Translucent Shader
		pShader = new CGraphicsShader;
		pShader->CreateVertexShader(L"shader\\ui.fx", "VS_UI");
		pShader->CreatePixelShader(L"shader\\ui.fx", "PS_UI");

		pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_FORWARD);
		pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
		pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
		pShader->SetRSType(RS_TYPE::CULL_BACK);

		pShader->AddTexParamInfo(L"UI TEXTURE", TEX_PARAM::TEX_0);
		pShader->AddScalarParamInfo(L"UI TYPE", SCALAR_PARAM::INT_0);
		pShader->AddScalarParamInfo(L"USE INFO", SCALAR_PARAM::INT_1);
		pShader->AddScalarParamInfo(L"USE PROGRESS BAR", SCALAR_PARAM::INT_2);
		pShader->AddScalarParamInfo(L"PROGRESS BAR DIRECTION", SCALAR_PARAM::INT_3);
		pShader->AddScalarParamInfo(L"PERCENTAGE", SCALAR_PARAM::FLOAT_0);
		pShader->AddScalarParamInfo(L"TEXTURE SIZE", SCALAR_PARAM::VEC2_0);
		pShader->AddScalarParamInfo(L"UI INFO", SCALAR_PARAM::VEC4_0);

		CResMgr::GetInst()->AddRes<CGraphicsShader>(L"UITranslucentShader", pShader, true);
	}

	void MakeGameMaterial()
	{
		CMaterial* pMtrl = nullptr;

		//SINE POST PROCESS MATERIAL
		pMtrl = new CMaterial{};
		pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"SineWavePostProcessShader"));
		CResMgr::GetInst()->AddRes<CMaterial>(L"material\\SineWaveMtrl.mtrl", pMtrl);

		//ABSORB POST PROCESS MATERIAL
		pMtrl = new CMaterial{};
		pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"AbsorbPostProcessShader"));
		CResMgr::GetInst()->AddRes<CMaterial>(L"material\\AbsorbMtrl.mtrl", pMtrl);

		//UI(DEFAULT)
		pMtrl = new CMaterial;
		pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"UIShader"));
		CResMgr::GetInst()->AddRes<CMaterial>(L"material\\UIMtrl.mtrl", pMtrl);

		//UI(ITEM)
		pMtrl = new CMaterial;
		pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"UIShader"));
		CResMgr::GetInst()->AddRes<CMaterial>(L"material\\UIItem.mtrl", pMtrl);

		pMtrl = new CMaterial;
		pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"UIShader"));
		CResMgr::GetInst()->AddRes<CMaterial>(L"material\\UIBoss.mtrl", pMtrl);

		//UI_INTERFACE1
		pMtrl = new CMaterial;
		pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"UITranslucentShader"));
		CResMgr::GetInst()->AddRes<CMaterial>(L"material\\UIInterface1.mtrl", pMtrl);

		//UI_INTERFACE2
		pMtrl = new CMaterial;
		pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"UITranslucentShader"));
		CResMgr::GetInst()->AddRes<CMaterial>(L"material\\UIInterface2.mtrl", pMtrl);

		//UI_PLAYER
		pMtrl = new CMaterial;
		pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"UIShader"));
		CResMgr::GetInst()->AddRes<CMaterial>(L"material\\UIPlayer.mtrl", pMtrl);
	}

	void MakeGameCompute() { }
}

void CResMgr::init()
{
	InitSound();
	//DirectXText에서 내부적으로 사용하는 COM으로 LoadXXX함수를 사용 이전에 초기화 필요
	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	CThreadPool::GetInst()->EnqueueJob([this]()
	{
		CreateEngineMesh();
		LOG_TRACE("ENGINE MESH CREATED");
	});
	CThreadPool::GetInst()->EnqueueJob([this]()
	{
		CreateEngineTexture();
		LOG_TRACE("ENGINE TEXTURE CREATED");
	});
	CThreadPool::GetInst()->EnqueueJob([this]()
	{
		CreateEngineShader();
		LOG_TRACE("ENGINE SHADER CREATED");
	});
	CThreadPool::GetInst()->EnqueueJob([this]()
	{
		CreateEngineComputeShader();
		LOG_TRACE("ENGINE COMPUTE-SHADER CREATED");
	});
	CThreadPool::GetInst()->Start();
	CThreadPool::GetInst()->Join();
	CreateEngineMaterial();
	LOG_TRACE("ENGINE MATERIAL CREATED");
}

void CResMgr::update()
{
	CSound::g_pFMOD->update();
}

void CResMgr::CreateEngineMesh()
{
	CMesh* pMesh = nullptr;

	vector<Vtx>  vecVtx;
	vector<UINT> vecIdx;

	Vtx v;
	// ==========
	// Point Mesh	
	// ==========
	v.vPos   = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUV    = Vec2(0.f, 0.f);
	vecVtx.push_back(v);
	vecIdx.push_back(0);

	pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(), static_cast<UINT>(vecIdx.size()));
	AddRes<CMesh>(L"PointMesh", pMesh, true);
	vecVtx.clear();
	vecIdx.clear();

	// ===============
	//	Polygon Mesh
	// 0 
	// |  \  
	// 2 -- 1
	// ===============
	v.vPos      = Vec3(-0.5f, 0.5f, 0.f);
	v.vColor    = Vec4(1.f, 0.2f, 0.2f, 1.f);
	v.vUV       = Vec2(0.f, 0.f);
	v.vTangent  = Vec3(1.f, 0.f, 0.f);
	v.vNormal   = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos      = Vec3(0.5f, -0.5f, 0.f);
	v.vColor    = Vec4(0.2f, 1.f, 0.2f, 1.f);
	v.vUV       = Vec2(1.f, 0.f);
	v.vTangent  = Vec3(1.f, 0.f, 0.f);
	v.vNormal   = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos      = Vec3(-0.5f, -0.5f, 0.f);
	v.vColor    = Vec4(1.f, 0.2f, 0.2f, 1.f);
	v.vUV       = Vec2(0.f, 1.f);
	v.vTangent  = Vec3(1.f, 0.f, 0.f);
	v.vNormal   = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);

	pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(), static_cast<UINT>(vecIdx.size()));
	AddRes<CMesh>(L"PolygonMesh", pMesh, true);

	vecVtx.clear();
	vecIdx.clear();

	// ========
	// RectMesh
	// 0 --- 1
	// |  \  |
	// 3 --- 2
	// ========
	v.vPos      = Vec3(-0.5f, 0.5f, 0.f);
	v.vColor    = Vec4(1.f, 0.2f, 0.2f, 1.f);
	v.vUV       = Vec2(0.f, 0.f);
	v.vTangent  = Vec3(1.f, 0.f, 0.f);
	v.vNormal   = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);

	vecVtx.push_back(v);

	v.vPos      = Vec3(0.5f, 0.5f, 0.f);
	v.vColor    = Vec4(0.2f, 1.f, 0.2f, 1.f);
	v.vUV       = Vec2(1.f, 0.f);
	v.vTangent  = Vec3(1.f, 0.f, 0.f);
	v.vNormal   = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos      = Vec3(0.5f, -0.5f, 0.f);
	v.vColor    = Vec4(0.2f, 0.2f, 1.f, 1.f);
	v.vUV       = Vec2(1.f, 1.f);
	v.vTangent  = Vec3(1.f, 0.f, 0.f);
	v.vNormal   = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos      = Vec3(-0.5f, -0.5f, 0.f);
	v.vColor    = Vec4(1.f, 0.2f, 0.2f, 1.f);
	v.vUV       = Vec2(0.f, 1.f);
	v.vTangent  = Vec3(1.f, 0.f, 0.f);
	v.vNormal   = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	vecIdx.push_back(0);
	vecIdx.push_back(2);
	vecIdx.push_back(3);
	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);

	pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(), static_cast<UINT>(vecIdx.size()));
	AddRes<CMesh>(L"RectMesh", pMesh, true);
	vecIdx.clear();

	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);
	vecIdx.push_back(3);
	vecIdx.push_back(0);

	pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(), static_cast<UINT>(vecIdx.size()));
	AddRes<CMesh>(L"RectMesh_LineStrip", pMesh, true);
	vecVtx.clear();
	vecIdx.clear();


	/*

		... 9 7 5 3 1
		... 8 6 4 2 0

	*/

	int iPolyGonCnt = 30;
	int iVtxCnt     = 4 + 2 * (iPolyGonCnt - 1);

	auto vUV_RT   = Vec2(1.f, 0.f);
	auto vUV_RB   = Vec2(1.f, 1.f);
	Vec2 vUV_Dist = Vec2(1.f, 1.f) / static_cast<float>(iPolyGonCnt);

	for (int i = 0; i < iVtxCnt; ++i)
	{
		v.vPos      = Vec3(0.f, 0.f, 0.f);
		v.vColor    = Vec4(1.f, 0.2f, 0.2f, 1.f);
		v.vTangent  = Vec3(1.f, 0.f, 0.f);
		v.vNormal   = Vec3(0.f, 0.f, -1.f);
		v.vBinormal = Vec3(0.f, 1.f, 0.f);

		if (i >= 2)
		{
			if (i % 2 == 1)
			{
				vUV_RT.x -= vUV_Dist.x;
				v.vUV = vUV_RT;
				if (i == iVtxCnt - 1) v.vUV = Vec2(0.f, vUV_RT.y);
			}
			else
			{
				vUV_RB.x -= vUV_Dist.x;
				v.vUV = vUV_RB;
				if (i == iVtxCnt - 2) v.vUV = Vec2(0.f, vUV_RB.y);
			}
		}
		else
		{
			if (i == 0) v.vUV = vUV_RB;
			if (i == 1) v.vUV = vUV_RT;
		}

		vecVtx.push_back(v);
	}

	for (int i = 0; i < iVtxCnt - 2; i += 2)
	{
		vecIdx.push_back(i + 3);
		vecIdx.push_back(i + 1);
		vecIdx.push_back(i);

		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 3);
		vecIdx.push_back(i);
	}

	pMesh = new CMesh;
	pMesh->CreateBuffer_Dynamic(vecVtx.data(),
	                            static_cast<UINT>(vecVtx.size()),
	                            vecIdx.data(),
	                            static_cast<UINT>(vecIdx.size()));
	AddRes<CMesh>(L"RectMesh_Trail", pMesh, true);
	vecVtx.clear();
	vecIdx.clear();

	// ==========
	// CircleMesh
	// ==========
	v.vPos   = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUV    = Vec2(0.5f, 0.5f);
	vecVtx.push_back(v);

	UINT  iSliceCount = 40;
	float fRadius     = 0.5f;
	float fAngleStep  = XM_2PI / static_cast<float>(iSliceCount);

	for (UINT i = 0; i < iSliceCount + 1; ++i)
	{
		v.vPos = Vec3(fRadius * cosf(fAngleStep * static_cast<float>(i)),
		              fRadius * sinf(fAngleStep * static_cast<float>(i)),
		              0.f);
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		v.vUV    = Vec2(v.vPos.x + 0.5f, -v.vPos.y + 0.5f);
		vecVtx.push_back(v);
	}


	for (UINT i = 0; i < iSliceCount + 1; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(), static_cast<UINT>(vecIdx.size()));
	AddRes<CMesh>(L"CircleMesh", pMesh, true);
	vecIdx.clear();

	// CicleMesh_LineStrip
	for (UINT i = 1; i <= iSliceCount + 1; ++i)
	{
		vecIdx.push_back(i);
	}

	pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(), static_cast<UINT>(vecIdx.size()));
	AddRes<CMesh>(L"CircleMesh_LineStrip", pMesh, true);
	vecVtx.clear();
	vecIdx.clear();


	// =========
	// Cube Mesh
	// =========
	Vtx arrCube[24] = {};
	// 윗면
	arrCube[0].vPos    = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[0].vColor  = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[0].vUV     = Vec2(0.f, 0.f);
	arrCube[0].vNormal = Vec3(0.f, 1.f, 0.f);

	arrCube[1].vPos    = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[1].vColor  = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[1].vUV     = Vec2(0.f, 0.f);
	arrCube[1].vNormal = Vec3(0.f, 1.f, 0.f);

	arrCube[2].vPos    = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[2].vColor  = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[2].vUV     = Vec2(0.f, 0.f);
	arrCube[2].vNormal = Vec3(0.f, 1.f, 0.f);

	arrCube[3].vPos    = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[3].vColor  = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[3].vUV     = Vec2(0.f, 0.f);
	arrCube[3].vNormal = Vec3(0.f, 1.f, 0.f);


	// 아랫 면	
	arrCube[4].vPos    = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[4].vColor  = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[4].vUV     = Vec2(0.f, 0.f);
	arrCube[4].vNormal = Vec3(0.f, -1.f, 0.f);

	arrCube[5].vPos    = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[5].vColor  = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[5].vUV     = Vec2(0.f, 0.f);
	arrCube[5].vNormal = Vec3(0.f, -1.f, 0.f);

	arrCube[6].vPos    = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[6].vColor  = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[6].vUV     = Vec2(0.f, 0.f);
	arrCube[6].vNormal = Vec3(0.f, -1.f, 0.f);

	arrCube[7].vPos    = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[7].vColor  = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[7].vUV     = Vec2(0.f, 0.f);
	arrCube[7].vNormal = Vec3(0.f, -1.f, 0.f);

	// 왼쪽 면
	arrCube[8].vPos    = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[8].vColor  = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[8].vUV     = Vec2(0.f, 0.f);
	arrCube[8].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrCube[9].vPos    = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[9].vColor  = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[9].vUV     = Vec2(0.f, 0.f);
	arrCube[9].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrCube[10].vPos    = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[10].vColor  = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[10].vUV     = Vec2(0.f, 0.f);
	arrCube[10].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrCube[11].vPos    = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[11].vColor  = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[11].vUV     = Vec2(0.f, 0.f);
	arrCube[11].vNormal = Vec3(-1.f, 0.f, 0.f);

	// 오른쪽 면
	arrCube[12].vPos    = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[12].vColor  = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[12].vUV     = Vec2(0.f, 0.f);
	arrCube[12].vNormal = Vec3(1.f, 0.f, 0.f);

	arrCube[13].vPos    = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[13].vColor  = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[13].vUV     = Vec2(0.f, 0.f);
	arrCube[13].vNormal = Vec3(1.f, 0.f, 0.f);

	arrCube[14].vPos    = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[14].vColor  = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[14].vUV     = Vec2(0.f, 0.f);
	arrCube[14].vNormal = Vec3(1.f, 0.f, 0.f);

	arrCube[15].vPos    = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[15].vColor  = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[15].vUV     = Vec2(0.f, 0.f);
	arrCube[15].vNormal = Vec3(1.f, 0.f, 0.f);

	// 뒷 면
	arrCube[16].vPos    = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[16].vColor  = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[16].vUV     = Vec2(0.f, 0.f);
	arrCube[16].vNormal = Vec3(0.f, 0.f, 1.f);

	arrCube[17].vPos    = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[17].vColor  = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[17].vUV     = Vec2(0.f, 0.f);
	arrCube[17].vNormal = Vec3(0.f, 0.f, 1.f);

	arrCube[18].vPos    = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[18].vColor  = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[18].vUV     = Vec2(0.f, 0.f);
	arrCube[18].vNormal = Vec3(0.f, 0.f, 1.f);

	arrCube[19].vPos    = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[19].vColor  = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[19].vUV     = Vec2(0.f, 0.f);
	arrCube[19].vNormal = Vec3(0.f, 0.f, 1.f);

	// 앞 면
	arrCube[20].vPos    = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[20].vColor  = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[20].vUV     = Vec2(0.f, 0.f);
	arrCube[20].vNormal = Vec3(0.f, 0.f, -1.f);

	arrCube[21].vPos    = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[21].vColor  = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[21].vUV     = Vec2(0.f, 0.f);
	arrCube[21].vNormal = Vec3(0.f, 0.f, -1.f);

	arrCube[22].vPos    = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[22].vColor  = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[22].vUV     = Vec2(0.f, 0.f);
	arrCube[22].vNormal = Vec3(0.f, 0.f, -1.f);

	arrCube[23].vPos    = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[23].vColor  = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[23].vUV     = Vec2(0.f, 0.f);
	arrCube[23].vNormal = Vec3(0.f, 0.f, -1.f);

	// 인덱스
	for (int i = 0; i < 12; i += 2)
	{
		vecIdx.push_back(i * 2);
		vecIdx.push_back(i * 2 + 1);
		vecIdx.push_back(i * 2 + 2);

		vecIdx.push_back(i * 2);
		vecIdx.push_back(i * 2 + 2);
		vecIdx.push_back(i * 2 + 3);
	}

	pMesh = new CMesh;
	pMesh->Create(arrCube, 24, vecIdx.data(), static_cast<UINT>(vecIdx.size()));
	AddRes(L"CubeMesh", pMesh, true);
	vecIdx.clear();


	// ===========
	// Sphere Mesh
	// ===========
	fRadius = 0.5f;

	// Top
	v.vPos    = Vec3(0.f, fRadius, 0.f);
	v.vUV     = Vec2(0.5f, 0.f);
	v.vColor  = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();
	v.vTangent  = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, 1.f);
	vecVtx.push_back(v);

	// Body
	UINT iStackCount = 40; // 가로 분할 개수
	iSliceCount      = 40; // 세로 분할 개수

	float fStackAngle = XM_PI / iStackCount;
	float fSliceAngle = XM_2PI / iSliceCount;

	float fUVXStep = 1.f / static_cast<float>(iSliceCount);
	float fUVYStep = 1.f / static_cast<float>(iStackCount);

	for (UINT i = 1; i < iStackCount; ++i)
	{
		float phi = i * fStackAngle;

		for (UINT j = 0; j <= iSliceCount; ++j)
		{
			float theta = j * fSliceAngle;

			v.vPos = Vec3(fRadius * sinf(i * fStackAngle) * cosf(j * fSliceAngle),
			              fRadius * cosf(i * fStackAngle),
			              fRadius * sinf(i * fStackAngle) * sinf(j * fSliceAngle));
			v.vUV     = Vec2(fUVXStep * j, fUVYStep * i);
			v.vColor  = Vec4(1.f, 1.f, 1.f, 1.f);
			v.vNormal = v.vPos;
			v.vNormal.Normalize();

			v.vTangent.x = -fRadius * sinf(phi) * sinf(theta);
			v.vTangent.y = 0.f;
			v.vTangent.z = fRadius * sinf(phi) * cosf(theta);
			v.vTangent.Normalize();

			v.vTangent.Cross(v.vNormal, v.vBinormal);
			v.vBinormal.Normalize();

			vecVtx.push_back(v);
		}
	}

	// Bottom
	v.vPos    = Vec3(0.f, -fRadius, 0.f);
	v.vUV     = Vec2(0.5f, 1.f);
	v.vColor  = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();

	v.vTangent  = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	// 인덱스
	// 북극점
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	// 몸통
	for (UINT i = 0; i < iStackCount - 2; ++i)
	{
		for (UINT j = 0; j < iSliceCount; ++j)
		{
			// + 
			// | \
			// +--+
			vecIdx.push_back((iSliceCount + 1) * (i) + (j) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j) + 1);

			// +--+
			//  \ |
			//    +
			vecIdx.push_back((iSliceCount + 1) * (i) + (j) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i) + (j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
		}
	}

	// 남극점
	UINT iBottomIdx = static_cast<UINT>(vecVtx.size()) - 1;
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(iBottomIdx);
		vecIdx.push_back(iBottomIdx - (i + 2));
		vecIdx.push_back(iBottomIdx - (i + 1));
	}

	pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(), static_cast<UINT>(vecIdx.size()));
	AddRes(L"SphereMesh", pMesh, true);

	vecVtx.clear();
	vecIdx.clear();

	// Cylinder

	// Frustum


	// ================
	// PentaHedron Mesh - For view Frustum 
	// ================
	// Normal 값 재정의 해야함 
	// ================
	/*
			  1 -- 0
			  |	   |
		4(0,0)|	   |
			  |    |
			  2	-- 3

	 */
	Vtx arrPenta2[16] = {};

	// 앞 면	
	arrPenta2[0].vPos    = Vec3(0.5f, 0.5f, 1.f);
	arrPenta2[0].vColor  = Vec4(1.f, 0.f, 0.f, 1.f);
	arrPenta2[0].vUV     = Vec2(0.f, 0.f);
	arrPenta2[0].vNormal = Vec3(0.f, 0.f, 1.f);

	arrPenta2[1].vPos    = Vec3(-0.5f, 0.5f, 1.f);
	arrPenta2[1].vColor  = Vec4(1.f, 0.f, 0.f, 1.f);
	arrPenta2[1].vUV     = Vec2(0.f, 0.f);
	arrPenta2[1].vNormal = Vec3(0.f, 0.f, 1.f);

	arrPenta2[2].vPos    = Vec3(-0.5f, -0.5f, 1.f);
	arrPenta2[2].vColor  = Vec4(1.f, 0.f, 0.f, 1.f);
	arrPenta2[2].vUV     = Vec2(0.f, 0.f);
	arrPenta2[2].vNormal = Vec3(0.f, 0.f, 1.f);

	arrPenta2[3].vPos    = Vec3(0.5f, -0.5f, 1.f);
	arrPenta2[3].vColor  = Vec4(1.f, 0.f, 0.f, 1.f);
	arrPenta2[3].vUV     = Vec2(0.f, 0.f);
	arrPenta2[3].vNormal = Vec3(0.f, 0.f, 1.f);

	// 윗 면
	arrPenta2[4].vPos    = Vec3(0.f, 0.f, 0.f);
	arrPenta2[4].vColor  = Vec4(0.f, 1.f, 0.f, 1.f);
	arrPenta2[4].vUV     = Vec2(0.f, 0.f);
	arrPenta2[4].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrPenta2[5].vPos    = Vec3(-0.5f, 0.5f, 1.f);
	arrPenta2[5].vColor  = Vec4(1.f, 0.f, 0.f, 1.f);
	arrPenta2[5].vUV     = Vec2(0.f, 0.f);
	arrPenta2[5].vNormal = Vec3(0.f, 0.f, 1.f);

	arrPenta2[6].vPos    = Vec3(0.5f, 0.5f, 1.f);
	arrPenta2[6].vColor  = Vec4(1.f, 0.f, 0.f, 1.f);
	arrPenta2[6].vUV     = Vec2(0.f, 0.f);
	arrPenta2[6].vNormal = Vec3(0.f, 0.f, 1.f);

	// 아랫 면
	arrPenta2[7].vPos    = Vec3(0.f, 0.f, 0.f);
	arrPenta2[7].vColor  = Vec4(0.f, 1.f, 0.f, 1.f);
	arrPenta2[7].vUV     = Vec2(0.f, 0.f);
	arrPenta2[7].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrPenta2[8].vPos    = Vec3(0.5f, 0.5f, 1.f);
	arrPenta2[8].vColor  = Vec4(1.f, 0.f, 0.f, 1.f);
	arrPenta2[8].vUV     = Vec2(0.f, 0.f);
	arrPenta2[8].vNormal = Vec3(0.f, 0.f, 1.f);

	arrPenta2[9].vPos    = Vec3(0.5f, -0.5f, 1.f);
	arrPenta2[9].vColor  = Vec4(1.f, 0.f, 0.f, 1.f);
	arrPenta2[9].vUV     = Vec2(0.f, 0.f);
	arrPenta2[9].vNormal = Vec3(0.f, 0.f, 1.f);


	// 왼쪽 면
	arrPenta2[10].vPos    = Vec3(0.f, 0.f, 0.f);
	arrPenta2[10].vColor  = Vec4(0.f, 1.f, 0.f, 1.f);
	arrPenta2[10].vUV     = Vec2(0.f, 0.f);
	arrPenta2[10].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrPenta2[11].vPos    = Vec3(-0.5f, -0.5f, 1.f);
	arrPenta2[11].vColor  = Vec4(1.f, 0.f, 0.f, 1.f);
	arrPenta2[11].vUV     = Vec2(0.f, 0.f);
	arrPenta2[11].vNormal = Vec3(0.f, 0.f, 1.f);

	arrPenta2[12].vPos    = Vec3(-0.5f, 0.5f, 1.f);
	arrPenta2[12].vColor  = Vec4(1.f, 0.f, 0.f, 1.f);
	arrPenta2[12].vUV     = Vec2(0.f, 0.f);
	arrPenta2[12].vNormal = Vec3(0.f, 0.f, 1.f);

	// 오른쪽 면
	arrPenta2[13].vPos    = Vec3(0.f, 0.f, 0.f);
	arrPenta2[13].vColor  = Vec4(0.f, 1.f, 0.f, 1.f);
	arrPenta2[13].vUV     = Vec2(0.f, 0.f);
	arrPenta2[13].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrPenta2[14].vPos    = Vec3(0.5f, 0.5f, 1.f);
	arrPenta2[14].vColor  = Vec4(1.f, 0.f, 0.f, 1.f);
	arrPenta2[14].vUV     = Vec2(0.f, 0.f);
	arrPenta2[14].vNormal = Vec3(0.f, 0.f, 1.f);

	arrPenta2[15].vPos    = Vec3(0.5f, -0.5f, 1.f);
	arrPenta2[15].vColor  = Vec4(1.f, 0.f, 0.f, 1.f);
	arrPenta2[15].vUV     = Vec2(0.f, 0.f);
	arrPenta2[15].vNormal = Vec3(0.f, 0.f, 1.f);

	// 인덱스
	for (int i = 0; i < 3; i += 1)
	{
		vecIdx.push_back(4);
		vecIdx.push_back(i + 1);
		vecIdx.push_back(i);
	}
	vecIdx.push_back(4);
	vecIdx.push_back(0);
	vecIdx.push_back(3);

	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(3);

	vecIdx.push_back(1);
	vecIdx.push_back(2);
	vecIdx.push_back(3);

	pMesh = new CMesh;
	pMesh->Create(arrPenta2, 16, vecIdx.data(), static_cast<UINT>(vecIdx.size()));
	AddRes(L"PentahedronMesh", pMesh, true);
	vecIdx.clear();
}

void CResMgr::CreateEngineTexture()
{
	Ptr<CTexture> pNoise01    = Load<CTexture>(L"texture\\noise\\noise_01.png", L"texture\\noise\\noise_01.png", true);
	Ptr<CTexture> pNoise02    = Load<CTexture>(L"texture\\noise\\noise_02.png", L"texture\\noise\\noise_02.png", true);
	Ptr<CTexture> pNoiseCloud = Load<CTexture>(L"texture\\noise\\noise_cloud.jpg",
	                                           L"texture\\noise\\noise_cloud.jpg",
	                                           true);

	pNoise01->UpdateData(ALL, 70);
	pNoise01->UpdateData_CS(70, true);

	pNoise02->UpdateData(ALL, 71);
	pNoise02->UpdateData_CS(71, true);

	pNoiseCloud->UpdateData(ALL, 72);
	pNoiseCloud->UpdateData_CS(72, true);

	g_global.vNoiseResolution = Vec2(pNoise01->Width(), pNoise01->Height());
}

void CResMgr::CreateEngineShader()
{
	MakeInputLayoutInfo();

	CGraphicsShader* pShader = nullptr;

	// Std2D Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_MASKED);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->AddScalarParamInfo(L"Mask Limit", SCALAR_PARAM::FLOAT_0);
	pShader->AddTexParamInfo(L"OutputTex 0", TEX_PARAM::TEX_0);
	pShader->AddTexParamInfo(L"OutputTex 1", TEX_PARAM::TEX_1);
	pShader->AddTexParamInfo(L"OutputTex 2", TEX_PARAM::TEX_2);
	pShader->AddScalarParamInfo(L"TestParam", SCALAR_PARAM::INT_0);
	AddRes<CGraphicsShader>(L"Std2DShader", pShader, true);

	// Std2DAlphaBlend Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2DAlpha");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2DAlpha");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TRANSLUCENT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->AddTexParamInfo(L"OutputTex", TEX_PARAM::TEX_0);
	AddRes<CGraphicsShader>(L"Std2DAlphaBlendShader", pShader, true);

	// Std3D Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\std3d.fx", "VS_Std3D");
	pShader->CreatePixelShader(L"shader\\std3d.fx", "PS_Std3D");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_MASKED);
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->AddScalarParamInfo(L"Mask Limit", SCALAR_PARAM::FLOAT_0);
	pShader->AddTexParamInfo(L"OutputTex", TEX_PARAM::TEX_0);
	pShader->AddTexParamInfo(L"NormapMap", TEX_PARAM::TEX_1);
	AddRes<CGraphicsShader>(L"Std3DShader", pShader, true);

	// Std3D Wire Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"Shader\\std3d.fx", "VS_Std3DWire");
	pShader->CreatePixelShader(L"Shader\\std3d.fx", "PS_Std3DWire");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_FORWARD);
	pShader->SetRSType(RS_TYPE::WIRE_FRAME);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	AddRes<CGraphicsShader>(L"Std3DWireShader", pShader, true);

	// Std3D Alpha Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"Shader\\std3d.fx", "VS_Std3DAlpha");
	pShader->CreatePixelShader(L"Shader\\std3d.fx", "PS_Std3DAlpha");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TRANSLUCENT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	AddRes<CGraphicsShader>(L"Std3DAlphaShader", pShader, true);

	// Std3D Deferred Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\std3d_deferred.fx", "VS_Std3D_Deferred");
	pShader->CreatePixelShader(L"shader\\std3d_deferred.fx", "PS_Std3D_Deferred");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_DEFERRED);
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->AddTexParamInfo(L"OutputTex", TEX_PARAM::TEX_0);
	pShader->AddTexParamInfo(L"NormapMap", TEX_PARAM::TEX_1);
	pShader->AddTexParamInfo(L"SpecularMap", TEX_PARAM::TEX_2);
	pShader->AddTexParamInfo(L"EmbientMap", TEX_PARAM::TEX_3);
	pShader->AddScalarParamInfo(L"HitOn", SCALAR_PARAM::INT_1);
	AddRes<CGraphicsShader>(L"Std3D_DeferredShader", pShader, true);

	// Std3D fog Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\std3d_deferred.fx", "VS_Std3D_fog");
	pShader->CreatePixelShader(L"shader\\std3d_deferred.fx", "PS_Std3D_fog");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TRANSLUCENT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::NO_ALPHA_COVERAGE);
	pShader->AddTexParamInfo(L"Output Texture", TEX_PARAM::TEX_0);
	pShader->AddTexParamInfo(L"Depth Texture", TEX_PARAM::TEX_1);
	AddRes<CGraphicsShader>(L"Std3D_fog", pShader, true);

	// Trail Shader 
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\trail.fx", "VS_Trail");
	pShader->CreatePixelShader(L"shader\\trail.fx", "PS_Trail");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TRANSLUCENT);
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::NO_ALPHA_COVERAGE);
	AddRes<CGraphicsShader>(L"TrailShader", pShader, true);

	// Sword TRail Shader 
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\trail.fx", "VS_SwordTrail");
	pShader->CreatePixelShader(L"shader\\trail.fx", "PS_SwordTrail");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TRANSLUCENT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::NO_ALPHA_COVERAGE);
	AddRes<CGraphicsShader>(L"SwordTrailShader", pShader, true);

	// PaperBurn Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\paperburn.fx", "VS_PaperBurn");
	pShader->CreatePixelShader(L"shader\\paperburn.fx", "PS_PaperBurn");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_MASKED);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->AddScalarParamInfo(L"Burn Strength", SCALAR_PARAM::FLOAT_0);
	pShader->AddTexParamInfo(L"OutputTex", TEX_PARAM::TEX_0);
	AddRes<CGraphicsShader>(L"PaperBurnShader", pShader, true);

	// PaperBurn_FullScreen Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\paperburn.fx", "VS_PaperBurnFullScreen");
	pShader->CreatePixelShader(L"shader\\paperburn.fx", "PS_PaperBurn");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::NO_ALPHA_COVERAGE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pShader->AddScalarParamInfo(L"Burn Strength", SCALAR_PARAM::FLOAT_0);
	pShader->AddTexParamInfo(L"OutputTex", TEX_PARAM::TEX_0);
	AddRes<CGraphicsShader>(L"PaperBurnFullScreenShader", pShader, true);

	// Column Flame Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\boss.fx", "VS_ColumnFlame");
	pShader->CreatePixelShader(L"shader\\boss.fx", "PS_ColumnFlame");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TRANSLUCENT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->AddTexParamInfo(L"OutputTex", TEX_PARAM::TEX_0);
	pShader->AddScalarParamInfo(L"Color", SCALAR_PARAM::VEC4_0);
	pShader->AddScalarParamInfo(L"Speed", SCALAR_PARAM::FLOAT_0);
	AddRes<CGraphicsShader>(L"ColumnFlameShader", pShader, true);

	// magma Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\boss.fx", "VS_Magma");
	pShader->CreatePixelShader(L"shader\\boss.fx", "PS_Magma");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_DEFERRED);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->AddTexParamInfo(L"OutputTex", TEX_PARAM::TEX_0);
	pShader->AddScalarParamInfo(L"Explode On", SCALAR_PARAM::FLOAT_0);
	AddRes<CGraphicsShader>(L"MagmaShader", pShader, true);

	// Pot Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\boss.fx", "VS_Pot");
	pShader->CreatePixelShader(L"shader\\boss.fx", "PS_Pot");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_DEFERRED);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->AddTexParamInfo(L"OutputTex", TEX_PARAM::TEX_0);
	pShader->AddTexParamInfo(L"NormapMap", TEX_PARAM::TEX_1);
	pShader->AddTexParamInfo(L"SpecularMap", TEX_PARAM::TEX_2);
	pShader->AddTexParamInfo(L"EmbientMap", TEX_PARAM::TEX_3);
	pShader->AddScalarParamInfo(L"Hit On", SCALAR_PARAM::INT_0);
	AddRes<CGraphicsShader>(L"PotShader", pShader, true);

	// EnergyBall Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\boss.fx", "VS_EnergyBall");
	pShader->CreatePixelShader(L"shader\\boss.fx", "PS_EnergyBall");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_FORWARD);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->AddTexParamInfo(L"OutputTex", TEX_PARAM::TEX_0);
	AddRes<CGraphicsShader>(L"EnergyBallShader", pShader, true);

	// TileMap Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\tilemap.fx", "VS_TileMap");
	pShader->CreatePixelShader(L"shader\\tilemap.fx", "PS_TileMap");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_MASKED);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->AddTexParamInfo(L"TileMapAtlas", TEX_PARAM::TEX_0);
	AddRes<CGraphicsShader>(L"TileMapShader", pShader, true);

	// Collider2D Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"Shader\\std2d.fx", "VS_Collider2D");
	pShader->CreatePixelShader(L"Shader\\std2d.fx", "PS_Collider2D");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TRANSLUCENT);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	AddRes<CGraphicsShader>(L"Collider2DShader", pShader, true);

	// Collider3D Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"Shader\\std2d.fx", "VS_Collider2D");
	pShader->CreatePixelShader(L"Shader\\std2d.fx", "PS_Collider2D");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_TRANSLUCENT);
	pShader->SetRSType(RS_TYPE::WIRE_FRAME);
	pShader->SetDSType(DS_TYPE::NO_TEST);
	pShader->AddScalarParamInfo(L"IsCollision", SCALAR_PARAM::INT_0);
	AddRes<CGraphicsShader>(L"Collider3DShader", pShader, true);

	// SkyBox Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"Shader\\std3d.fx", "VS_SkyBox");
	pShader->CreatePixelShader(L"Shader\\std3d.fx", "PS_SkyBox");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_FORWARD);
	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetDSType(DS_TYPE::LESS_EQUAL);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->AddTexParamInfo(L"Output Texture", TEX_PARAM::TEX_0);
	pShader->AddTexParamInfo(L"Cube Texture", TEX_PARAM::TEX_CUBE_0);
	AddRes<CGraphicsShader>(L"SkyBoxShader", pShader, true);

	// Particle Render Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"Shader\\particlerender.fx", "VS_ParticleRender");
	pShader->CreateGeometryShader(L"Shader\\particlerender.fx", "GS_ParticleRender");
	pShader->CreatePixelShader(L"Shader\\particlerender.fx", "PS_ParticleRender");
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_EMISSIVE_PARTICLE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::NO_ALPHA_COVERAGE);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	AddRes<CGraphicsShader>(L"ParticleRenderShader", pShader, true);

	// PostProcess Shader
	pShader = new CGraphicsShader;
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_PostProcess");
	pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_PostProcess");
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	AddRes<CGraphicsShader>(L"PostProcessShader", pShader, true);

	// FlamePostProcess Shader
	pShader = new CGraphicsShader;
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_FlamePostProcess");
	pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_FlamePostProcess");
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->AddScalarParamInfo(L"Speed", SCALAR_PARAM::FLOAT_0);
	AddRes<CGraphicsShader>(L"FlamePostProcessShader", pShader, true);

	// Fog PostProcess Shader
	pShader = new CGraphicsShader;
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_FogPostProcess");
	pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_FogPostProcess");
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->AddTexParamInfo(L"Render Target", TEX_PARAM::TEX_0);
	pShader->AddTexParamInfo(L"Position Target", TEX_PARAM::TEX_1);
	pShader->AddScalarParamInfo(L"Fog Color", SCALAR_PARAM::VEC4_0);
	AddRes<CGraphicsShader>(L"FogPostProcessShader", pShader, true);

	// Emissive PostProcess Shader
	pShader = new CGraphicsShader;
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_EmissivePostProcess");
	pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_EmissivePostProcess");
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->AddTexParamInfo(L"Render Target", TEX_PARAM::TEX_0);
	pShader->AddTexParamInfo(L"Emissive Target", TEX_PARAM::TEX_1);
	AddRes<CGraphicsShader>(L"EmissivePostProcessShader", pShader, true);

	// Tessellation Test Shader
	pShader = new CGraphicsShader;
	pShader->SetShaderDomain(SHADER_DOMAIN::DOMAIN_FORWARD);
	pShader->CreateVertexShader(L"Shader\\tessellation.fx", "VS_Tess");
	pShader->CreateHullShader(L"Shader\\tessellation.fx", "HS_Tess");
	pShader->CreateDomainShader(L"Shader\\tessellation.fx", "DS_Tess");
	pShader->CreatePixelShader(L"Shader\\tessellation.fx", "PS_Tess");
	pShader->SetRSType(RS_TYPE::WIRE_FRAME);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	AddRes<CGraphicsShader>(L"TessShader", pShader, true);

	MakeGameShader();
}

void CResMgr::CreateEngineMaterial()
{
	CMaterial* pMtrl = nullptr;

	// Std2DMtrl 생성
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DShader"));
	AddRes<CMaterial>(L"material\\Std2DMtrl.mtrl", pMtrl);

	// Std2DAlphaBlend
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DAlphaBlendShader"));
	AddRes<CMaterial>(L"material\\Std2DAlphaBlendMtrl.mtrl", pMtrl);

	// Std3D Material
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std3DShader"));
	AddRes<CMaterial>(L"material\\Std3DMtrl.mtrl", pMtrl);

	// Std3D Wire
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std3DWireShader"));
	AddRes<CMaterial>(L"material\\Std3DWireShader.mtrl", pMtrl);

	// Std3D Alpha
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std3DAlphaShader"));
	AddRes<CMaterial>(L"material\\Std3DAlphaMtrl.mtrl", pMtrl);

	// Collider3D
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Collider3DShader"));
	AddRes<CMaterial>(L"material\\Collider3D.mtrl", pMtrl);
	Vec4 color = {1.f, 0.f, 1.f, 1.f};
	pMtrl->SetScalarParam(SCALAR_PARAM::VEC4_0, &color);

	// Std3D Deferred Material
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std3D_DeferredShader"));
	AddRes<CMaterial>(L"material\\Std3D_DeferredMtrl.mtrl", pMtrl);

	// SkyBox Material
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"SkyBoxShader"));
	AddRes<CMaterial>(L"material\\SkyBoxMtrl.mtrl", pMtrl);

	// Trail Material
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"TrailShader"));
	AddRes<CMaterial>(L"material\\TrailMtrl.mtrl", pMtrl);

	// Sword Trail Material
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"SwordTrailShader"));
	AddRes<CMaterial>(L"material\\SwordTrailMtrl.mtrl", pMtrl);

	// PaperBurnMtrl	
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"PaperBurnShader"));
	AddRes<CMaterial>(L"material\\PaperBurnMtrl.mtrl", pMtrl);

	// PaperBurnFullScreenMtrl	
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"PaperBurnFullScreenShader"));
	AddRes<CMaterial>(L"material\\PaperBurnFullScreenMtrl.mtrl", pMtrl);

	// TileMapMtrl
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"TileMapShader"));
	AddRes<CMaterial>(L"material\\TileMapMtrl.mtrl", pMtrl);

	// ColumnFlameMtrl
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"ColumnFlameShader"));
	AddRes<CMaterial>(L"material\\ColumnFlameMtrl.mtrl", pMtrl);

	// MagmaMtrl
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"MagmaShader"));
	AddRes<CMaterial>(L"material\\MagmaMtrl.mtrl", pMtrl);

	// PotMtrl
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"PotShader"));
	AddRes<CMaterial>(L"material\\PotMtrl.mtrl", pMtrl);

	// EnergyBallMtrl
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"EnergyBallShader"));
	AddRes<CMaterial>(L"material\\EnergyBallMtrl.mtrl", pMtrl);

	// Collider2DMtrl 
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Collider2DShader"));
	AddRes<CMaterial>(L"material\\Collider2DMtrl.mtrl", pMtrl);

	// Particle Render Mtrl
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"ParticleRenderShader"));
	AddRes<CMaterial>(L"material\\ParticleRenderMtrl.mtrl", pMtrl);

	// PostProcess Mtrl
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"PostProcessShader"));
	AddRes<CMaterial>(L"material\\PostProcessMtrl.mtrl", pMtrl);
	

	// FlamePostProcessMtrl
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"FlamePostProcessShader"));
	AddRes<CMaterial>(L"material\\FlamePostProcessMtrl.mtrl", pMtrl);

	// Fog PostProcess Mtrl
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"FogPostProcessShader"));
	AddRes<CMaterial>(L"material\\FogPostProcessMtrl.mtrl", pMtrl);

	// Emissive PostProcess Mtrl
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"EmissivePostProcessShader"));
	AddRes<CMaterial>(L"material\\EmissivePostProcessMtrl.mtrl", pMtrl);

	// Tessellation Test Material
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"TessShader"));
	AddRes<CMaterial>(L"material\\TessMtrl.mtrl", pMtrl);

	// fog Test Material
	pMtrl = new CMaterial;
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std3D_fog"));
	AddRes<CMaterial>(L"material\\fog.mtrl", pMtrl);

	MakeGameMaterial();
}

void CResMgr::CreateEngineComputeShader()
{
	CComputeShader* pCS = nullptr;

	// TestShader
	pCS = new CTestShader;
	pCS->CreateComputeShader(L"shader\\testcs.fx", "CS_Test");
	AddRes<CComputeShader>(L"TestCS", pCS, true);

	// Particle Update Shader
	pCS = new CParticleUpdateShader;
	pCS->CreateComputeShader(L"shader\\particle.fx", "CS_Particle");
	AddRes<CComputeShader>(L"ParticleUpdateShader", pCS, true);

	// Animation3D Update Shader	
	pCS = new CAnimation3DShader;
	pCS->CreateComputeShader(L"shader\\animation3d.fx", "CS_Animation3D");
	AddRes<CComputeShader>(L"Animation3DUpdateShader", pCS, true);

	MakeGameCompute();
}

void CResMgr::MakeInputLayoutInfo()
{
	// 정점 입력 구조 (InputLayout 생성)	
	UINT                     iOffset    = 0;
	D3D11_INPUT_ELEMENT_DESC tInputDesc = {};

	tInputDesc.SemanticName         = "POSITION";	// Semantic 이름
	tInputDesc.SemanticIndex        = 0;			// 중복 이름인 경우 인덱스로 구분
	tInputDesc.InputSlot            = 0;
	tInputDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	tInputDesc.InstanceDataStepRate = 0;
	tInputDesc.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	tInputDesc.AlignedByteOffset    = iOffset;
	iOffset += 12;
	CGraphicsShader::AddInputLayout(tInputDesc);

	tInputDesc.SemanticName         = "COLOR";
	tInputDesc.SemanticIndex        = 0;
	tInputDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	tInputDesc.InstanceDataStepRate = 0;
	tInputDesc.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tInputDesc.AlignedByteOffset    = iOffset;
	iOffset += 16;
	CGraphicsShader::AddInputLayout(tInputDesc);

	tInputDesc.SemanticName         = "TEXCOORD";
	tInputDesc.SemanticIndex        = 0;
	tInputDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	tInputDesc.InstanceDataStepRate = 0;
	tInputDesc.Format               = DXGI_FORMAT_R32G32_FLOAT;
	tInputDesc.AlignedByteOffset    = iOffset;
	iOffset += 8;
	CGraphicsShader::AddInputLayout(tInputDesc);

	tInputDesc.SemanticName         = "TANGENT";
	tInputDesc.SemanticIndex        = 0;
	tInputDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	tInputDesc.InstanceDataStepRate = 0;
	tInputDesc.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	tInputDesc.AlignedByteOffset    = iOffset;
	iOffset += 12;
	CGraphicsShader::AddInputLayout(tInputDesc);

	tInputDesc.SemanticName         = "NORMAL";
	tInputDesc.SemanticIndex        = 0;
	tInputDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	tInputDesc.InstanceDataStepRate = 0;
	tInputDesc.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	tInputDesc.AlignedByteOffset    = iOffset;
	iOffset += 12;
	CGraphicsShader::AddInputLayout(tInputDesc);

	tInputDesc.SemanticName         = "BINORMAL";
	tInputDesc.SemanticIndex        = 0;
	tInputDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	tInputDesc.InstanceDataStepRate = 0;
	tInputDesc.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	tInputDesc.AlignedByteOffset    = iOffset;
	iOffset += 12;
	CGraphicsShader::AddInputLayout(tInputDesc);

	tInputDesc.SemanticName         = "BLENDWEIGHT";
	tInputDesc.SemanticIndex        = 0;
	tInputDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	tInputDesc.InstanceDataStepRate = 0;
	tInputDesc.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tInputDesc.AlignedByteOffset    = iOffset;
	iOffset += 16;
	CGraphicsShader::AddInputLayout(tInputDesc);

	tInputDesc.SemanticName         = "BLENDINDICES";
	tInputDesc.SemanticIndex        = 0;
	tInputDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	tInputDesc.InstanceDataStepRate = 0;
	tInputDesc.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tInputDesc.AlignedByteOffset    = iOffset;
	iOffset += 16;
	CGraphicsShader::AddInputLayout(tInputDesc);
}

void CResMgr::InitSound()
{
	System_Create(&CSound::g_pFMOD);
	LOG_ASSERT(CSound::g_pFMOD, "INIT FMOD FAILED");

	// 32개 채널 생성
	CSound::g_pFMOD->init(32, FMOD_DEFAULT, nullptr);
}
