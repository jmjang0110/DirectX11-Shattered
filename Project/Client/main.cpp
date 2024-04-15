#include "pch.h"

#include <Engine/CCore.h>
#include <Engine/CDevice.h>
#include <Engine/CPrefab.h>

#include <Script/CSceneSaveLoad.h>

#include "CImGuiMgr.h"
#include "ImGui/imgui_impl_win32.h"
#include "CToolObjMgr.h"
#include "CTestScene.h"


HINSTANCE hInst;
HWND      g_hWnd;

ATOM             MyRegisterClass(HINSTANCE hInstance);
BOOL             InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE     hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR        lpCmdLine,
                      _In_ int           nCmdShow)
{
#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	AllocConsole();
	CLog::Init();
	LOG_INFO("PROGRAM START");
	LOG_TRACE("CONSOLE OPENED");
#else
    #define DBG_NEW new
#endif

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		LOG_ERROR("WINDOW CREATION FAILED");
		return EXIT_FAILURE;
	}
	LOG_INFO("WINDOW CREATED");

	if (FAILED(CCore::GetInst()->init(g_hWnd, POINT{ 1600, 900 })))
	{
		LOG_ERROR("CCORE INIT FAILED");
		return EXIT_FAILURE;
	}
	LOG_INFO("CCORE INIT SUCCEDED");

	CPrefab::m_pSaveFunc = &CSceneSaveLoad::SavePrefab;
	CPrefab::m_pLoadFunc = &CSceneSaveLoad::LoadPrefab;

	CToolObjMgr::GetInst()->init();
	LOG_INFO("TOOL MANAGER INIT SUCCEDED");

	CImGuiMgr::GetInst()->init(g_hWnd);
	LOG_INFO("IMGUI INIT SUCCEDED");

	CTestScene::CreateTestScene();
	LOG_INFO("SCENE CREATED");

	LOG_TRACE("ENTERING GAME LOOP");
	MSG msg{};
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (WM_QUIT == msg.message)
			{
				break;
			}
		}
		else
		{
			CCore::GetInst()->progress();
			CToolObjMgr::GetInst()->progress();
			CImGuiMgr::GetInst()->progress();
			CImGuiMgr::GetInst()->render();
			CDevice::GetInst()->Present();
		}
	}
	LOG_TRACE("EXIT GAME LOOP");

	CImGuiMgr::GetInst()->clear();
	LOG_INFO("CLEAR IMGUI");

	LOG_INFO("QUIT PROGRAM");
	return static_cast<int>(msg.wParam);
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex{};
	wcex.cbSize        = sizeof(WNDCLASSEX);
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = WndProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = hInstance;
	wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY39ENGINE));
	wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
	wcex.lpszMenuName  = nullptr;
	wcex.lpszClassName = L"WindowClass";
	wcex.hIconSm       = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	g_hWnd = CreateWindowW(L"WindowClass",
	                       L"39Engine",
	                       WS_OVERLAPPEDWINDOW,
	                       CW_USEDEFAULT,
	                       0,
	                       CW_USEDEFAULT,
	                       0,
	                       nullptr,
	                       nullptr,
	                       hInstance,
	                       nullptr);
	if (nullptr == g_hWnd)
	{
		return FALSE;
	}
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return TRUE;
}

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			FreeConsole();
		}
		break;

	case WM_DPICHANGED:
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			const RECT* suggested_rect = reinterpret_cast<RECT*>(lParam);
			SetWindowPos(hWnd,
			             nullptr,
			             suggested_rect->left,
			             suggested_rect->top,
			             suggested_rect->right - suggested_rect->left,
			             suggested_rect->bottom - suggested_rect->top,
			             SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
