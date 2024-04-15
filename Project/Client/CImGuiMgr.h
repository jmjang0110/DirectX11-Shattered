#pragma once

#include "UI.h"

using PARAM_1 = void(UI::*)(DWORD_PTR);

struct tUIDelegate
{
	UI*       pInst;
	PARAM_1   pFunc;
	DWORD_PTR dwParam;
};


class CImGuiMgr
	: public CSingleton<CImGuiMgr>
{
	SINGLE(CImGuiMgr)
private:
	map<string, UI*>    m_mapUI;
	vector<tUIDelegate> m_vecDelegate;

	HANDLE m_hNotify;

public:
	void init(HWND _hwnd);

	void progress();
	void render();
	void clear();

private:
	void CreateUI();
	void ObserveContent();

public:
	UI*  FindUI(const string& _strKey);
	void AddDelegate(tUIDelegate _del) { m_vecDelegate.push_back(_del); }
};
