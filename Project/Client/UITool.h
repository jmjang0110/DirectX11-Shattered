#pragma once
#include "UI.h"

class CUIBase;
class CUIText;
class CUIProgressBar;
class CUIButton;
class CUIImage;
class CUIPanel;
class TreeNode;
class TreeUI;

enum class UI_TYPE
{
	PANEL,
	IMAGE,
	TEXT,

	END
};

enum class UI_EXTRA_TYPE
{
	BUTTON,
	PROGRESS_BAR,

	END
};

class UITool final
	: public UI
{
private:
	inline static UINT s_geneatedID = 0;
private:
	Ptr<CTexture> m_pFrameTexture = nullptr;
	CGameObject*  m_pSelected     = nullptr;

	bool   m_isDrawing  = false;
	bool   m_isDragging = false;
	ImVec2 m_startPos;
	ImVec2 m_lastPos;

	ImVec2 m_size;
	ImVec2 m_leftTop;

	bool m_bHasDragArea      = false;
	bool m_bShowWarning      = false;
	bool m_bWrongPrefab      = false;
	bool m_bAlreadyHas       = false;
	bool m_bNoImageScript    = false;
	bool m_bOpenWarningPopUp = false;
	bool m_bOpenFramePopUp   = false;
private:
	void DrawImage();
	void CropImageByDrag();
	void DrawInfo();
	void DrawWarning();
private:
	void ShowDefault(CUIBase* pScript) const;
	void ShowPanel(CUIPanel* pScript);
	void ShowImage(CUIImage* pScript);
	void ShowText(CUIText* pScript);
	void ShowButton(CUIButton* pScript) const;
	void ShowProgressBar(CUIProgressBar* pScript);

private:
	void SaveAsPrefab();
	void LoadUIPrefab();
private:
	void AddDelegate(DWORD_PTR _pGameObject);
	void AddUI(UI_TYPE type);
	void AddExtraFeature(UI_EXTRA_TYPE type);

	void FontSelected(DWORD_PTR _pFontName) const;
	void TextureSelected(DWORD_PTR _pTextureName) const;

	void CalculatePosAndSize(const Vec2& windowSize);
public:
	void render_update() override;
public:
	UITool();
	virtual ~UITool();
};
