#pragma once

class CConstBuffer;

class CDevice
	: public CSingleton<CDevice>
{
	SINGLE(CDevice);
private:
	HWND m_hWnd;              // Main Window Handle
	Vec2 m_vRenderResolution; // 렌더링 해상도

	ComPtr<ID3D11Device>        m_pDevice;        // GPU 메모리 제어
	ComPtr<ID3D11DeviceContext> m_pDeviceContext; // GPU Rendering 제어

	ComPtr<IDXGISwapChain> m_pSwapChain;
	DXGI_SWAP_CHAIN_DESC   m_tSwapChainDesc;

	ComPtr<ID3D11RasterizerState>   m_arrRS[static_cast<UINT>(RS_TYPE::END)];
	ComPtr<ID3D11DepthStencilState> m_arrDS[static_cast<UINT>(DS_TYPE::END)];
	ComPtr<ID3D11BlendState>        m_arrBS[static_cast<UINT>(BS_TYPE::END)];
	ComPtr<ID3D11SamplerState>      m_arrSam[static_cast<UINT>(SAMPLER_TYPE::END)];
	CConstBuffer* m_arrCB[static_cast<UINT>(CB_TYPE::END)];

	//Direct2D
	ComPtr<ID2D1RenderTarget> m_pRtv2D;
	ComPtr<ID2D1Factory>      m_pFactory2D;

	bool m_isVsyncEnabled;

public:
	void SetVsync(bool _enable) { m_isVsyncEnabled = _enable; }
	bool IsVSyncEnabled() const { return m_isVsyncEnabled; }
public:
	ComPtr<ID2D1RenderTarget> GetRtv2D() const { return m_pRtv2D; }
	ComPtr<ID2D1Factory>      GetFactory2D() const { return m_pFactory2D; }

public:
	int  init(HWND _hWnd, Vec2 _vRenderResolution);
	void Present() const { m_pSwapChain->Present(m_isVsyncEnabled, 0); }

	Vec2 GetRenderResolution() const { return m_vRenderResolution; }

	ComPtr<ID3D11Device>        GetDevice() { return m_pDevice; }
	ComPtr<ID3D11DeviceContext> GetDeviceContext() { return m_pDeviceContext; }

	ComPtr<ID3D11RasterizerState>   GetRS(RS_TYPE _eType) { return m_arrRS[static_cast<UINT>(_eType)]; }
	ComPtr<ID3D11DepthStencilState> GetDS(DS_TYPE _eType) { return m_arrDS[static_cast<UINT>(_eType)]; }
	ComPtr<ID3D11BlendState>        GetBS(BS_TYPE _eType) { return m_arrBS[static_cast<UINT>(_eType)]; }
	CConstBuffer* GetCB(CB_TYPE _eType) const { return m_arrCB[static_cast<UINT>(_eType)]; }

private:
	int  CreateSwapchain();
	int  CreateView() const;
	int  CreateRasterizerState();
	int  CreateDepthStencilState();
	int  CreateBlendState();
	int  CreateConstBuffer();
	void CreateSamplerState();

	void UpdateSamplerState(int            _iRegisterNum,
		SAMPLER_TYPE   _eSamplerType,
		PIPELINE_STAGE _PipelineStage,
		bool           _bComputeShaderStage = true);


	//Direct2D
	void Init2D();
};
