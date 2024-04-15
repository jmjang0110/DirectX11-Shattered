#pragma once
#include "UI.h"

class MenuUI final
	: public UI
{
private:
	string m_strPlayPause;

	bool m_bPackaging;
	bool m_bSceneSave;
	bool m_bSceneLoad;
	bool m_bScenePlayPause;
	bool m_bSceneStop;
	bool m_bParticleTool;
	bool m_bFogTool;
	bool m_bUITool;

public:
	void update() override;
	void render() override;
	void render_update() override;

private:
	void Task();

public:
	MenuUI();
	virtual ~MenuUI();
};
