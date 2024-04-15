#include "pch.h"
#include "CDevice.h"

#include "CConstBuffer.h"

#include "CResMgr.h"

CDevice::CDevice()
	: m_hWnd(nullptr)
	, m_tSwapChainDesc{}
	, m_arrRS{}
	, m_arrCB{}
	, m_isVsyncEnabled{false} {}

CDevice::~CDevice()
{
	Safe_Del_Arr(m_arrCB);
}

int CDevice::init(HWND _hWnd, Vec2 _vRenderResolution)
{
	m_hWnd               = _hWnd;
	m_vRenderResolution  = _vRenderResolution;
	g_global.vResolution = m_vRenderResolution;


	UINT iFlag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	iFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL iFeautureLevel = D3D_FEATURE_LEVEL_11_0;

	const HRESULT hr = D3D11CreateDevice(nullptr,
	                                     D3D_DRIVER_TYPE_HARDWARE,
	                                     nullptr,
	                                     iFlag,
	                                     nullptr,
	                                     0,
	                                     D3D11_SDK_VERSION,
	                                     m_pDevice.GetAddressOf(),
	                                     &iFeautureLevel,
	                                     m_pDeviceContext.GetAddressOf());

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"장치 초기화 실패", L"엔진 초기화 실패", MB_OK);
		LOG_ERROR("DEVICE CREATION FAILED");
		return E_FAIL;
	}
	LOG_TRACE("DEVICE CREATED");

	UINT iQuality = 0;
	m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &iQuality);

	if (0 == iQuality)
	{
		MessageBox(nullptr, L"멀티셈플 레벨체크 실패", L"엔진 초기화 실패", MB_OK);
		LOG_ERROR("MULTI-SAMPLING CHECK FAILED");
		return E_FAIL;
	}
	LOG_TRACE("MULTI-SAMPLING CHECK SUCCEEDED");

	// SwapChain 생성
	if (FAILED(CreateSwapchain()))
	{
		MessageBox(nullptr, L"스왑체인 생성 실패", L"엔진 초기화 실패", MB_OK);
		LOG_ERROR("SWAPCHAIN CREATION FAILED");
		return E_FAIL;
	}
	LOG_TRACE("SWAPCHAIN CREATED");

	// View
	if (FAILED(CreateView()))
	{
		MessageBox(nullptr, L"뷰 생성 실패", L"엔진 초기화 실패", MB_OK);
		LOG_ERROR("SWAPCHAIN RTV/DSV CREATION FAILED");
		return E_FAIL;
	}
	LOG_TRACE("RTV/DSV CREATED");

	// 래스터라이저 스테이트 생성
	if (FAILED(CreateRasterizerState()))
	{
		LOG_ERROR("RASTERAZATION STATES CREATION FAILED");
		return E_FAIL;
	}
	LOG_TRACE("RASTERAZATION STATES CREATED");

	// 뎊스스텐실 스테이트 생성
	if (FAILED(CreateDepthStencilState()))
	{
		LOG_ERROR("DEPTH-STENCIL STATE CREATION FAILED");
		return E_FAIL;
	}
	LOG_TRACE("DEPTH_STENCIL STATES CREATED");

	// 블렌드 스테이트 생성
	if (FAILED(CreateBlendState()))
	{
		LOG_ERROR("BLEND-STATE CREATION FAILED");
		return E_FAIL;
	}
	LOG_TRACE("BLEND-STATES CREATED");

	// 상수버퍼 생성
	if (FAILED(CreateConstBuffer()))
	{
		LOG_ERROR("CONSTANT BUFFER CREATION FAILED");
		return E_FAIL;
	}
	LOG_TRACE("CONSTANT BUFFERS CREATED");

	// Sampler 생성
	CreateSamplerState();
	LOG_TRACE("SAMPLER STATED CREATED");

	Init2D();

	return S_OK;
}

int CDevice::CreateSwapchain()
{
	DXGI_SWAP_CHAIN_DESC desc               = {};
	desc.BufferCount                        = 1;
	desc.BufferDesc.Width                   = static_cast<UINT>(m_vRenderResolution.x);
	desc.BufferDesc.Height                  = static_cast<UINT>(m_vRenderResolution.y);
	desc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferDesc.RefreshRate.Numerator   = 60;
	desc.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.Flags                              = 0;
	desc.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
	desc.SampleDesc.Count                   = 1;
	desc.SampleDesc.Quality                 = 0;
	desc.OutputWindow                       = m_hWnd;
	desc.Windowed                           = true;


	ComPtr<IDXGIDevice>  pDXGIDevice  = nullptr;
	ComPtr<IDXGIAdapter> pDXGIAdaptor = nullptr;
	ComPtr<IDXGIFactory> pDXGIFactory = nullptr;

	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)pDXGIDevice.GetAddressOf());
	pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)pDXGIAdaptor.GetAddressOf());
	pDXGIAdaptor->GetParent(__uuidof(IDXGIFactory), (void**)pDXGIFactory.GetAddressOf());
	pDXGIFactory->CreateSwapChain(m_pDevice.Get(), &desc, m_pSwapChain.GetAddressOf());

	if (nullptr == m_pSwapChain)
	{
		return E_FAIL;
	}
	return S_OK;
}

int CDevice::CreateView() const
{
	// Render Target Texture	
	ComPtr<ID3D11Texture2D> pBuffer = nullptr;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)pBuffer.GetAddressOf());
	CResMgr::GetInst()->CreateTexture(L"RenderTargetTex", pBuffer, true);
	LOG_TRACE("RenderTargetTex CREATED");
	// Depth Stencil Texture 만들기
	Ptr<CTexture> pDepthStencilTex = CResMgr::GetInst()->CreateTexture(L"DepthStencilTex",
	                                                                   static_cast<UINT>(m_vRenderResolution.x),
	                                                                   static_cast<UINT>(m_vRenderResolution.y),
	                                                                   DXGI_FORMAT_D24_UNORM_S8_UINT,
	                                                                   D3D11_BIND_DEPTH_STENCIL,
	                                                                   true);
	LOG_TRACE("DepthStencilTex CREATED");
	return S_OK;
}

int CDevice::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC desc = {};
	HRESULT               hr   = S_OK;

	// Default State
	// 반시계(뒷면) 제외, 시계방향(앞면) 통과
	m_arrRS[static_cast<UINT>(RS_TYPE::CULL_BACK)] = nullptr;


	// 반시계(뒷면) 통과, 시계방향(앞면) 제외
	desc.CullMode = D3D11_CULL_FRONT;
	desc.FillMode = D3D11_FILL_SOLID;
	hr = DEVICE->CreateRasterizerState(&desc, m_arrRS[static_cast<UINT>(RS_TYPE::CULL_FRONT)].GetAddressOf());
	if (FAILED(hr))
	{
		LOG_ERROR("RS[CULL_BACK] CREATION FAIED");
		return E_FAIL;
	}

	// 양면 모두 그리기, (주로 단면 형태의 메쉬를 앞 뒤에서 볼때)
	desc.CullMode = D3D11_CULL_NONE;
	desc.FillMode = D3D11_FILL_SOLID;
	hr            = DEVICE->CreateRasterizerState(&desc, m_arrRS[static_cast<UINT>(RS_TYPE::CULL_NONE)].GetAddressOf());
	if (FAILED(hr))
	{
		LOG_ERROR("RS[CULL_NONE] CREATION FAILED");
		return E_FAIL;
	}

	// 양면 모두 그리기, 뼈대 픽셀만 렌더링
	desc.CullMode = D3D11_CULL_NONE;
	desc.FillMode = D3D11_FILL_WIREFRAME;
	hr = DEVICE->CreateRasterizerState(&desc, m_arrRS[static_cast<UINT>(RS_TYPE::WIRE_FRAME)].GetAddressOf());
	if (FAILED(hr))
	{
		LOG_ERROR("RS[WIRE_FRAME] CREATION FAILED");
		return E_FAIL;
	}

	return S_OK;
}

int CDevice::CreateDepthStencilState()
{
	// Less (Default)
	m_arrDS[static_cast<UINT>(DS_TYPE::LESS)] = nullptr;


	// LessEqual
	D3D11_DEPTH_STENCIL_DESC desc = {};
	desc.DepthEnable              = true;
	desc.DepthFunc                = D3D11_COMPARISON_LESS_EQUAL;
	desc.DepthWriteMask           = D3D11_DEPTH_WRITE_MASK_ALL;

	if (FAILED(DEVICE->CreateDepthStencilState(&desc, m_arrDS[static_cast<UINT>(DS_TYPE::LESS_EQUAL)].GetAddressOf())))
	{
		LOG_ERROR("DS[LESS_EQUAL] CREAION FAIED");
		return E_FAIL;
	}


	// Greater
	desc.DepthEnable    = true;
	desc.DepthFunc      = D3D11_COMPARISON_GREATER;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	if (FAILED(DEVICE->CreateDepthStencilState(&desc, m_arrDS[static_cast<UINT>(DS_TYPE::GREATER)].GetAddressOf())))
	{
		LOG_ERROR("DS[GREATER] CREAION FAIED");
		return E_FAIL;
	}

	// GreaterEqual
	desc.DepthEnable    = true;
	desc.DepthFunc      = D3D11_COMPARISON_GREATER_EQUAL;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	if (FAILED(DEVICE->CreateDepthStencilState(&desc, m_arrDS[static_cast<UINT>(DS_TYPE::GREATER_EQUAL)].GetAddressOf()
	           )))
	{
		LOG_ERROR("DS[GREATER_EQUAL] CREAION FAIED");
		return E_FAIL;
	}


	// No Test
	desc.DepthEnable    = false;
	desc.DepthFunc      = D3D11_COMPARISON_ALWAYS;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	if (FAILED(DEVICE->CreateDepthStencilState(&desc, m_arrDS[static_cast<UINT>(DS_TYPE::NO_TEST)].GetAddressOf())))
	{
		LOG_ERROR("DS[NO_TEST] CREAION FAIED");
		return E_FAIL;
	}


	// No Write
	desc.DepthEnable    = true;
	desc.DepthFunc      = D3D11_COMPARISON_LESS;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	if (FAILED(DEVICE->CreateDepthStencilState(&desc, m_arrDS[static_cast<UINT>(DS_TYPE::NO_WRITE)].GetAddressOf())))
	{
		LOG_ERROR("DS[NO_WRITE] CREAION FAIED");
		return E_FAIL;
	}


	// No Test No Write
	desc.DepthEnable    = false;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	if (FAILED(DEVICE->CreateDepthStencilState(&desc, m_arrDS[static_cast<UINT>(DS_TYPE::NO_TEST_NO_WRITE)].GetAddressOf
		           ())))
	{
		LOG_ERROR("DS[NO_TEST_NO_WRITE] CREAION FAIED");
		return E_FAIL;
	}


	// 후면 체크
	// RS_TYPE::CULL_FRONT
	desc.DepthEnable                 = true;
	desc.DepthFunc                   = D3D11_COMPARISON_GREATER;    // 깊이 검사 0
	desc.DepthWriteMask              = D3D11_DEPTH_WRITE_MASK_ZERO; // 깊이 기록 X	
	desc.StencilEnable               = true;
	desc.BackFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;
	desc.BackFace.StencilPassOp      = D3D11_STENCIL_OP_REPLACE;
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	desc.BackFace.StencilFailOp      = D3D11_STENCIL_OP_ZERO;

	//desc.FrontFace.StencilFunc;
	//desc.FrontFace.StencilPassOp;
	//desc.FrontFace.StencilDepthFailOp;
	//desc.FrontFace.StencilFailOp;


	// 전면 체크
	// RS_TYPE::CULL_BACK
	desc.DepthEnable                  = true;
	desc.DepthFunc                    = D3D11_COMPARISON_LESS;       // 깊이 검사 0
	desc.DepthWriteMask               = D3D11_DEPTH_WRITE_MASK_ZERO; // 깊이 기록 X	
	desc.StencilEnable                = true;
	desc.FrontFace.StencilFunc        = D3D11_COMPARISON_EQUAL;
	desc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	desc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_ZERO;


	/*desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;*/


	// 지정된 스텐실 영역만 렌더링
	desc.DepthEnable                  = false;
	desc.DepthWriteMask               = D3D11_DEPTH_WRITE_MASK_ZERO; // 깊이 기록 X	
	desc.StencilEnable                = true;
	desc.FrontFace.StencilFunc        = D3D11_COMPARISON_EQUAL;
	desc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_ZERO;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	desc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_ZERO;


	return S_OK;
}


int CDevice::CreateBlendState()
{
	m_arrBS[static_cast<UINT>(BS_TYPE::DEFAULT)] = nullptr;

	D3D11_BLEND_DESC desc                      = {};
	desc.AlphaToCoverageEnable                 = true;						// 커버레이지 옵션 사용 유무
	desc.IndependentBlendEnable                = false;						// 렌더타겟 블랜드스테이드 독립실행
	desc.RenderTarget[0].BlendEnable           = true;                      // 블랜딩 스테이트 사용
	desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;        // 가산 혼합
	desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;     // SrcRGB 블랜드 계수 ==> (SrcA)
	desc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA; // DestRGB 블랜드 계수 ==> (1 - SrcA)	
	desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(DEVICE->CreateBlendState(&desc, m_arrBS[static_cast<UINT>(BS_TYPE::ALPHA_BLEND)].GetAddressOf())))
	{
		LOG_ERROR("BS[ALPHA_BLEND] CREAION FAIED");
		return E_FAIL;
	}

	desc                                = {};
	desc.AlphaToCoverageEnable          = false;				// 커버레이지 옵션 사용 유무
	desc.IndependentBlendEnable         = false;				// 렌더타겟 블랜드스테이드 독립실행
	desc.RenderTarget[0].BlendEnable    = true;					// 블랜딩 스테이트 사용
	desc.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;	// 가산 혼합
	desc.RenderTarget[0].SrcBlend       = D3D11_BLEND_ONE;		// SrcRGB 블랜드 계수 ==> 1
	desc.RenderTarget[0].DestBlend      = D3D11_BLEND_ONE;		// DestRGB 블랜드 계수 ==> 1	
	desc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(DEVICE->CreateBlendState(&desc, m_arrBS[static_cast<UINT>(BS_TYPE::ONEONE_BLEND)].GetAddressOf())))
	{
		LOG_ERROR("BS[ONE_ONE_BLEND] CREAION FAIED");
		return E_FAIL;
	}

	desc                                       = {};
	desc.AlphaToCoverageEnable                 = FALSE;						// 커버레이지 옵션 사용 유무
	desc.IndependentBlendEnable                = TRUE;						// 렌더타겟 블랜드스테이드 독립실행
	desc.RenderTarget[0].BlendEnable           = true;                      // 블랜딩 스테이트 사용
	desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;        // 가산 혼합
	desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;     // SrcRGB 블랜드 계수 ==> (SrcA)
	desc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA; // DestRGB 블랜드 계수 ==> (1 - SrcA)	
	desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(DEVICE->CreateBlendState(&desc, m_arrBS[static_cast<UINT>(BS_TYPE::NO_ALPHA_COVERAGE)].GetAddressOf())))
	{
		LOG_ERROR("BS[NO_ALPHA_COVERAGE] CREAION FAIED");
		return E_FAIL;
	}
	return S_OK;
}

int CDevice::CreateConstBuffer()
{
	m_arrCB[static_cast<UINT>(CB_TYPE::TRANSFORM)] = new CConstBuffer(CB_TYPE::TRANSFORM);
	m_arrCB[static_cast<UINT>(CB_TYPE::TRANSFORM)]->Create(sizeof(tTransform));
	LOG_TRACE("CB_TYPE::TRANSFORM CREATED, SIZE:%d", sizeof(tTransform));

	m_arrCB[static_cast<UINT>(CB_TYPE::SCALAR_PARAM)] = new CConstBuffer(CB_TYPE::SCALAR_PARAM);
	m_arrCB[static_cast<UINT>(CB_TYPE::SCALAR_PARAM)]->Create(sizeof(tScalarParam));
	LOG_TRACE("CB_TYPE::SCALAR_PARAM CREATED, SIZE:%d", sizeof(SCALAR_PARAM));

	m_arrCB[static_cast<UINT>(CB_TYPE::ANIM2D)] = new CConstBuffer(CB_TYPE::ANIM2D);
	m_arrCB[static_cast<UINT>(CB_TYPE::ANIM2D)]->Create(sizeof(tAnim2D));
	LOG_TRACE("CB_TYPE::ANIM2D CREATED, SIZE:%d", sizeof(tAnim2D));

	m_arrCB[static_cast<UINT>(CB_TYPE::GLOBAL)] = new CConstBuffer(CB_TYPE::GLOBAL);
	m_arrCB[static_cast<UINT>(CB_TYPE::GLOBAL)]->Create(sizeof(tGlobal));
	LOG_TRACE("CB_TYPE::GLOBAL CREATED, SIZE:%d", sizeof(tGlobal));

	m_arrCB[static_cast<UINT>(CB_TYPE::SSAO_CHANGES_ON_RESIZE)] = new CConstBuffer(CB_TYPE::SSAO_CHANGES_ON_RESIZE);
	m_arrCB[static_cast<UINT>(CB_TYPE::SSAO_CHANGES_ON_RESIZE)]->Create(sizeof(tSSAO_ChangesOnResize));
	LOG_TRACE("CB_TYPE::SSAO_CHANGES_ON_RESIZE CREATED, SIZE:%d", sizeof(tSSAO_ChangesOnResize));

	m_arrCB[static_cast<UINT>(CB_TYPE::SSAO_CHANGES_RARELY)] = new CConstBuffer(CB_TYPE::SSAO_CHANGES_RARELY);
	m_arrCB[static_cast<UINT>(CB_TYPE::SSAO_CHANGES_RARELY)]->Create(sizeof(tSSAO_ChangesRarely));
	LOG_TRACE("CB_TYPE::SSAO_CHANGES_RARELY CREATED, SIZE:%d", sizeof(tSSAO_ChangesRarely));

	return S_OK;
}

void CDevice::CreateSamplerState()
{
	// ******************
	// 샘플러 상태 초기화
	// ******************

	D3D11_SAMPLER_DESC tDesc = {};

	//ANISOTROPIC_2X_WRAP - 기존 g_sam_0
	tDesc.Filter        = D3D11_FILTER_ANISOTROPIC;
	tDesc.MaxAnisotropy = 2;
	tDesc.AddressU      = D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.AddressV      = D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.AddressW      = D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.MaxLOD        = D3D11_FLOAT32_MAX;

	DEVICE->CreateSamplerState(&tDesc, m_arrSam[static_cast<UINT>(SAMPLER_TYPE::ANISOTROPIC_2X_WRAP)].GetAddressOf());

	// POINT_WRAP
	tDesc.Filter   = D3D11_FILTER_MIN_MAG_MIP_POINT;
	tDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.MaxLOD   = D3D11_FLOAT32_MAX;

	DEVICE->CreateSamplerState(&tDesc, m_arrSam[static_cast<UINT>(SAMPLER_TYPE::POINT_WRAP)].GetAddressOf());

	// 선형 필터링 및 클램프 모드
	CD3D11_SAMPLER_DESC sampDesc(CD3D11_DEFAULT{});
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	DEVICE->CreateSamplerState(&sampDesc, m_arrSam[static_cast<UINT>(SAMPLER_TYPE::LINEAR_CLAMP)].GetAddressOf());

	// 포인트 필터링 및 클램프 모드
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	DEVICE->CreateSamplerState(&tDesc, m_arrSam[static_cast<UINT>(SAMPLER_TYPE::POINT_CLAMP)].GetAddressOf());

	/*
		x4로 설정하면 비등방성 필터링이 텍스처의 모양을 결정하기 위해 텍셀당 4개의 샘플을 수집합니다
	*/

	// 2x 비등방성 필터링 및 클램프 모드
	sampDesc.Filter        = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 2;
	DEVICE->CreateSamplerState(&sampDesc,
	                           m_arrSam[static_cast<UINT>(SAMPLER_TYPE::ANISOTROPIC_2X_CLAMP)].GetAddressOf());

	// 4x 비등방성 필터링 및 클램프 모드
	sampDesc.MaxAnisotropy = 4;
	DEVICE->CreateSamplerState(&sampDesc,
	                           m_arrSam[static_cast<UINT>(SAMPLER_TYPE::ANISOTROPIC_4X_CLAMP)].GetAddressOf());

	// 8x 비등방성 필터링 및 클램프 모드
	sampDesc.MaxAnisotropy = 8;
	DEVICE->CreateSamplerState(&sampDesc,
	                           m_arrSam[static_cast<UINT>(SAMPLER_TYPE::ANISOTROPIC_8X_CLAMP)].GetAddressOf());

	// 16x 비등방성 필터링 및 클램프 모드
	sampDesc.MaxAnisotropy = 16;
	DEVICE->CreateSamplerState(&sampDesc,
	                           m_arrSam[static_cast<UINT>(SAMPLER_TYPE::ANISOTROPIC_16X_CLAMP)].GetAddressOf());

	// 선형 필터링 및 랩 모드
	sampDesc.Filter        = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU      = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV      = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW      = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MaxAnisotropy = 0;
	DEVICE->CreateSamplerState(&sampDesc, m_arrSam[static_cast<UINT>(SAMPLER_TYPE::LINEAR_WRAP)].GetAddressOf());

	//샘플러 상태: 깊이 비교 및 ​​경계 모드
	sampDesc.Filter         = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	sampDesc.AddressU       = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV       = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW       = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	sampDesc.MaxAnisotropy  = 1;
	sampDesc.MinLOD         = 0.0f;
	sampDesc.MaxLOD         = 0.0f;
	sampDesc.BorderColor[0] = 0.0f;
	sampDesc.BorderColor[1] = 0.0f;
	sampDesc.BorderColor[2] = 0.0f;
	sampDesc.BorderColor[3] = 1.0f;
	DEVICE->CreateSamplerState(&sampDesc, m_arrSam[static_cast<UINT>(SAMPLER_TYPE::SHADOW_PCF)].GetAddressOf());


	D3D11_SAMPLER_DESC samplerDesc; // SSAO
	ZeroMemory(&samplerDesc, sizeof samplerDesc);

	// 일반 및 깊이 샘플러
	samplerDesc.Filter         = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU       = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[3] = 1e5f;	// 매우 큰 깊이 값을 설정 (Normal, depthZ) = (0, 0, 0, 1e5f)
	samplerDesc.MinLOD         = 0.0f;
	samplerDesc.MaxLOD         = D3D11_FLOAT32_MAX;
	DEVICE->CreateSamplerState(&sampDesc, m_arrSam[static_cast<UINT>(SAMPLER_TYPE::NORMAL_DEPTH)].GetAddressOf());


	// 랜덤 벡터 샘플러
	samplerDesc.AddressU       = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.BorderColor[3] = 0.0f;
	DEVICE->CreateSamplerState(&sampDesc, m_arrSam[static_cast<UINT>(SAMPLER_TYPE::RANDOM_VEC)].GetAddressOf());


	// 블러링을 위한 샘플러
	samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	DEVICE->CreateSamplerState(&sampDesc, m_arrSam[static_cast<UINT>(SAMPLER_TYPE::BLUR)].GetAddressOf());


	// CPU -> GPU Update Sampler State
	UpdateSamplerState(static_cast<int>(SAMPLER_TYPE::ANISOTROPIC_2X_WRAP), SAMPLER_TYPE::ANISOTROPIC_2X_WRAP, ALL);
	LOG_TRACE("SAMPLER_TYPE::ANISOTROPIC_2X_WRAP BINDED :%d", SAMPLER_TYPE::ANISOTROPIC_2X_WRAP);

	UpdateSamplerState(static_cast<int>(SAMPLER_TYPE::POINT_WRAP), SAMPLER_TYPE::POINT_WRAP, ALL);
	LOG_TRACE("SAMPLER_TYPE::POINT_WRAP BINDED :%d", SAMPLER_TYPE::POINT_WRAP);

	UpdateSamplerState(static_cast<int>(SAMPLER_TYPE::LINEAR_CLAMP), SAMPLER_TYPE::LINEAR_CLAMP, ALL);
	LOG_TRACE("SAMPLER_TYPE::LINEAR_CLAMP BINDED :%d", SAMPLER_TYPE::LINEAR_CLAMP);

	UpdateSamplerState(static_cast<int>(SAMPLER_TYPE::POINT_CLAMP), SAMPLER_TYPE::POINT_CLAMP, ALL);
	LOG_TRACE("SAMPLER_TYPE::POINT_CLAMP BINDED :%d", SAMPLER_TYPE::POINT_CLAMP);

	UpdateSamplerState(static_cast<int>(SAMPLER_TYPE::ANISOTROPIC_2X_CLAMP), SAMPLER_TYPE::ANISOTROPIC_2X_CLAMP, ALL);
	LOG_TRACE("SAMPLER_TYPE::ANISOTROPIC_2X_CLAMP BINDED :%d", SAMPLER_TYPE::ANISOTROPIC_2X_CLAMP);

	UpdateSamplerState(static_cast<int>(SAMPLER_TYPE::ANISOTROPIC_4X_CLAMP), SAMPLER_TYPE::ANISOTROPIC_4X_CLAMP, ALL);
	LOG_TRACE("SAMPLER_TYPE::ANISOTROPIC_4X_CLAMP BINDED :%d", SAMPLER_TYPE::ANISOTROPIC_4X_CLAMP);

	UpdateSamplerState(static_cast<int>(SAMPLER_TYPE::ANISOTROPIC_8X_CLAMP), SAMPLER_TYPE::ANISOTROPIC_8X_CLAMP, ALL);
	LOG_TRACE("SAMPLER_TYPE::ANISOTROPIC_8X_CLAMP BINDED :%d", SAMPLER_TYPE::ANISOTROPIC_8X_CLAMP);

	UpdateSamplerState(static_cast<int>(SAMPLER_TYPE::ANISOTROPIC_16X_CLAMP), SAMPLER_TYPE::ANISOTROPIC_16X_CLAMP, ALL);
	LOG_TRACE("SAMPLER_TYPE::ANISOTROPIC_16X_CLAMP BINDED :%d", SAMPLER_TYPE::ANISOTROPIC_16X_CLAMP);

	UpdateSamplerState(static_cast<int>(SAMPLER_TYPE::ANISOTROPIC_16X_WRAP), SAMPLER_TYPE::ANISOTROPIC_16X_WRAP, ALL);
	LOG_TRACE("SAMPLER_TYPE::ANISOTROPIC_16X_WRAP BINDED :%d", SAMPLER_TYPE::ANISOTROPIC_16X_WRAP);

	UpdateSamplerState(static_cast<int>(SAMPLER_TYPE::LINEAR_WRAP), SAMPLER_TYPE::LINEAR_WRAP, ALL);
	LOG_TRACE("SAMPLER_TYPE::LINEAR_WRAP BINDED :%d", SAMPLER_TYPE::LINEAR_WRAP);

	UpdateSamplerState(static_cast<int>(SAMPLER_TYPE::SHADOW_PCF), SAMPLER_TYPE::SHADOW_PCF, ALL);
	LOG_TRACE("SAMPLER_TYPE::SHADOW_PCF BINDED :%d", SAMPLER_TYPE::SHADOW_PCF);

	UpdateSamplerState(static_cast<int>(SAMPLER_TYPE::NORMAL_DEPTH), SAMPLER_TYPE::NORMAL_DEPTH, ALL);
	LOG_TRACE("SAMPLER_TYPE::NORMAL_DEPTH BINDED :%d", SAMPLER_TYPE::NORMAL_DEPTH);

	UpdateSamplerState(static_cast<int>(SAMPLER_TYPE::RANDOM_VEC), SAMPLER_TYPE::RANDOM_VEC, ALL);
	LOG_TRACE("SAMPLER_TYPE::RANDOM_VEC BINDED :%d", SAMPLER_TYPE::RANDOM_VEC);

	UpdateSamplerState(static_cast<int>(SAMPLER_TYPE::BLUR), SAMPLER_TYPE::BLUR, ALL);
	LOG_TRACE("SAMPLER_TYPE::BLUR BINDED :%d", SAMPLER_TYPE::BLUR);
}

void CDevice::Init2D()
{
	// 2D Factory 생성
	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, m_pFactory2D.GetAddressOf())))
	{
		LOG_ERROR("DIRECTX 2D FACTORY CREATION FAILED");
	}
	LOG_INFO("D2D FACTORY CREATED");

	// 3D BackBuffer의 Surface를 얻어온다.
	ComPtr<IDXGISurface> pBackBuffer{};

	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

	// 2D용 렌더타겟을 만들어준다.
	const D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_HARDWARE,
	                                                                         D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN,
		                                                                         D2D1_ALPHA_MODE_PREMULTIPLIED));

	if (FAILED(m_pFactory2D->CreateDxgiSurfaceRenderTarget(pBackBuffer.Get(), props, m_pRtv2D.GetAddressOf())))
	{
		LOG_ERROR("DIRECTX RTV2D CREATION FAILED");
	}
	LOG_INFO("RTV2D CREATED");
}

void CDevice::UpdateSamplerState(int            _iRegisterNum,
                                 SAMPLER_TYPE   _eSamplerType,
                                 PIPELINE_STAGE _PipelineStage,
                                 bool           _bComputeShaderStage)
{
	if (_PipelineStage & static_cast<UINT>(PIPELINE_STAGE::VS))
		CONTEXT->VSSetSamplers(_iRegisterNum, 1, m_arrSam[static_cast<UINT>(_eSamplerType)].GetAddressOf());

	if (_PipelineStage & static_cast<UINT>(PIPELINE_STAGE::HS))
		CONTEXT->HSSetSamplers(_iRegisterNum, 1, m_arrSam[static_cast<UINT>(_eSamplerType)].GetAddressOf());

	if (_PipelineStage & static_cast<UINT>(PIPELINE_STAGE::DS))
		CONTEXT->DSSetSamplers(_iRegisterNum, 1, m_arrSam[static_cast<UINT>(_eSamplerType)].GetAddressOf());

	if (_PipelineStage & static_cast<UINT>(PIPELINE_STAGE::GS))
		CONTEXT->GSSetSamplers(_iRegisterNum, 1, m_arrSam[static_cast<UINT>(_eSamplerType)].GetAddressOf());

	if (_PipelineStage & static_cast<UINT>(PIPELINE_STAGE::PS))
		CONTEXT->PSSetSamplers(_iRegisterNum, 1, m_arrSam[static_cast<UINT>(_eSamplerType)].GetAddressOf());

	if (_bComputeShaderStage)
		CONTEXT->CSSetSamplers(_iRegisterNum, 1, m_arrSam[static_cast<UINT>(_eSamplerType)].GetAddressOf());
}
