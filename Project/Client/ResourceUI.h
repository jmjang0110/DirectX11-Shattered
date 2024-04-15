#pragma once
#include "UI.h"

#include <Engine/CResMgr.h>
class TreeUI;

class ResourceUI
	: public UI
{
private:
	TreeUI*         m_TreeUI;
	vector<wstring> m_vecResPath;

public:
	void update() override;
	void render_update() override;

public:
	void Reset();

private:
	void Reload();
	void Renew();
	void FindFileName(const wstring& _strFolderPath);

	void ItemClicked(DWORD_PTR _dwNode);
	void ItemDBClicked(DWORD_PTR _dwNode);

	RES_TYPE GetResTypeFromExt(const wstring& _strExt);

private:
	void UsingMultiThread();
	void WithOutMultiThead();

	template <typename T>
	void LoadResource(const std::wstring& resourceKey)
	{
		assert(RES_TYPE::END != CResMgr::GetInst()->GetResType<T>() && "INVALID TYPE");
		CResMgr::GetInst()->Load<T>(resourceKey, resourceKey);
	}

public:
	ResourceUI();
	virtual ~ResourceUI();
};
