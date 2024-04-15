#include "pch.h"
#include "CCore.h"

#include "CDevice.h"
#include "CTimeMgr.h"
#include "CPathMgr.h"
#include "CKeyMgr.h"
#include "CResMgr.h"
#include "CSceneMgr.h"
#include "CRenderMgr.h"
#include "CEventMgr.h"
#include "CFontMgr.h"
#include "CRenderEffectMgr.h"

CCore::CCore()
	: m_hWnd(nullptr)
	, m_ptResolution{} {}

CCore::~CCore() = default;

int CCore::init(HWND _hWnd, POINT _ptResolution)
{
	RECT rt = {0, 0, _ptResolution.x, _ptResolution.y};
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(_hWnd, nullptr, 10, 10, rt.right - rt.left, rt.bottom - rt.top, 0);

	LOG_TRACE("CLIENT RESOLUTION : %d, %d", _ptResolution.x, _ptResolution.y);
	LOG_TRACE("WINDOW SIZE ADJUSTED : BEFORE(%d,%d) -> AFTER(%d,%d)",
	          _ptResolution.x,
	          _ptResolution.y,
	          rt.right - rt.left,
	          rt.bottom - rt.top);

	m_hWnd         = _hWnd;
	m_ptResolution = _ptResolution;

	// Manager ÃÊ±âÈ­
	if (FAILED(CDevice::GetInst()->init(m_hWnd, Vec2(static_cast<float>(m_ptResolution.x), static_cast<float>(
		           m_ptResolution.y)))))
	{
		LOG_ERROR("CDEVICE INIT FAILED");
		return E_FAIL;
	}
	LOG_INFO("CDEVICE INIT SUCCEDED");

	CPathMgr::GetInst()->init();
	LOG_INFO("PATH MANAGER INIT SUCCEED");
	CKeyMgr::GetInst()->init();
	LOG_INFO("KEY MANAGER INIT SUCCEED");
	CTimeMgr::GetInst()->init();
	LOG_INFO("TIME MANAGER INIT SUCCEED");
	CResMgr::GetInst()->init();
	LOG_INFO("RESOURCE MANAGER INIT SUCCEED");
	CRenderMgr::GetInst()->init();
	LOG_INFO("RENDER MANAGER INIT SUCCEED");
	CRenderEffectMgr::GetInst()->Init();
	LOG_INFO("RENDER-EFFECT MANAGER INIT SUCCEED");
	CSceneMgr::GetInst()->init();
	LOG_INFO("SCENE MANAGER INIT SUCCEED");
	CFontMgr::GetInst()->Init();
	LOG_INFO("FONT MANAGER INIT SUCCEED");

	return S_OK;
}


void CCore::Frame_Init()
{
	CTimeMgr::GetInst()->update();
	CKeyMgr::GetInst()->update();
}

void CCore::Frame_Clear() {}

void CCore::progress()
{
	Frame_Init();

	CSceneMgr::GetInst()->progress();
	CResMgr::GetInst()->update();
	CRenderMgr::GetInst()->render();
	CEventMgr::GetInst()->update();
	Frame_Clear();
}
