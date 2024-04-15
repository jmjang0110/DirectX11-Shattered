#pragma once
#include <Engine/CScript.h>


class MainMenuScript
	: public CScript
{
	enum MENU_TYPE
	{
		E_START,
		E_SETTING,
		E_LEAVE,
		E_OPTION,
		E_BACK,

		E_END
	};

private:
	std::vector<CGameObject*> m_vecSelectionMenu;
	std::vector<CGameObject*> m_vecOptionMenu;
	CGameObject*              m_pPlayerUI;
	std::bitset<E_END>        m_bitSelected;

private:
	void MainLoading();
	void OptionLoading();

private:
	void FindChild();
	void OnHover(CGameObject* pGameObject);

private:
	void LeaveGame(CGameObject* pGameObject);
	void StartGame(CGameObject* pGameObject);
	void SettingGame(CGameObject* pGameObject);

private:
	void OnFXAA(CGameObject* pGameObject);
	void OnSSAO(CGameObject* pGameObject);
	void OnVSync(CGameObject* pGameObject);
	void OnBack(CGameObject* pGameObject);

public:
	void start() override;
	void update() override;

public:
	CLONE(MainMenuScript);
	MainMenuScript();
	virtual ~MainMenuScript();
};
