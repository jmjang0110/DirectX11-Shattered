#pragma once
#include "CScript.h"

#include "CSceneMgr.h"
#include "CScene.h"
#include <tuple>

#undef max
class CSingletonScript
	: public CScript
{
	friend class CEventMgr;

protected:
	inline static CGameObject*                       s_pInstance = nullptr;
	inline static std::vector<std::function<void()>> s_vecFunc{};

public:
	template <typename T, typename... Args>
	static void AddScriptEvent(T* pInstance, void (T::*callback)(Args...), Args... args)
	{
		auto arguments = std::make_tuple(pInstance, std::forward<decltype(args)>(args)...);
		auto func      = [callback,arguments = std::move(arguments)]()
		{
			std::apply(callback, arguments);
		};
		s_vecFunc.push_back(std::move(func));
	}

	static void FireScriptEvents()
	{
		for (auto& func : s_vecFunc)
		{
			func();
		}
		s_vecFunc.clear();
	}

public:
	CSingletonScript()
		: CScript{std::numeric_limits<int>::max()} { }

	virtual ~CSingletonScript() { s_pInstance = nullptr; }
	CLONE(CSingletonScript);
};

template <typename T>
class CMgrScript : public CSingletonScript
{
public:
	static T* GetInst()
	{
		if (nullptr == s_pInstance)
		{
			s_pInstance = new CGameObject{};
			s_pInstance->SetName(L"ManagerScript");
			CSceneMgr::GetInst()->GetCurScene()->AddObject(s_pInstance, 0);
		}

		T* pScript = s_pInstance->GetScript<T>();
		if (nullptr == pScript)
		{
			pScript = new T{};
			s_pInstance->AddComponent(pScript);
			if (CSceneMgr::GetInst()->GetCurScene()->GetSceneState() == SCENE_STATE::PLAY)
			{
				pScript->start();
			}
			return pScript;
		}
		return pScript;
	}
};
