#pragma once
#include "UI.h"

#include <Engine/CKeyMgr.h>

class TreeUI;


// DWORD_PTR TreeNode::m_dwData 데이터실체 명시
enum class NODE_TYPE
{
	// default
	NONE,
	DUMMY,

	ENGINE_SCENE,
	ENGINE_LAYER,
	ENGINE_GAMEOBJECT,
	ENGINE_COMPONENT,
	ENGINE_RESOURCE,

	END,
};

class TreeNode
{
private:
	TreeUI*           m_pTreeUI;
	TreeNode*         m_pParent;
	vector<TreeNode*> m_vecChild;
	bool              m_bLeaf;
	bool              m_bSelected;

	string    m_strName;
	DWORD_PTR m_dwData;

private:
	NODE_TYPE m_eNodeType;
	bool      m_bCheckOn;

public:
	void      SetNodeType(NODE_TYPE _type) { m_eNodeType = _type; }
	NODE_TYPE GetNodeType() { return m_eNodeType; }

	void RenderSelectedMark(ImU32 _color);
public:
	void update();
	void render_update();

public:
	void AddChild(TreeNode* _pChild)
	{
		_pChild->m_pParent = this;
		m_vecChild.push_back(_pChild);
		m_bLeaf = false;
	}

	const string& GetName() { return m_strName; }
	DWORD_PTR     GetData() { return m_dwData; }
	TreeNode*     GetParent() { return m_pParent; }
	void          SetCheckOn() { m_bCheckOn = true; }
public:
	TreeNode();
	TreeNode(const string& _strName, DWORD_PTR _dwData);
	~TreeNode();

	friend class TreeUI;
};


using CLICKED = void(UI::*)(DWORD_PTR);
using DRAG_DROP = void(UI::*)(DWORD_PTR, DWORD_PTR);
using KEY_FUNC = CLICKED;
using DROP = void(UI::*)(DWORD_PTR);

struct tTreeKey
{
	KEY     eKey;
	UI*     pInst;
	CLICKED pFunc;
};


class TreeUI : public UI
{
private:
	TreeNode* m_pRootNode;
	TreeNode* m_pSelectedNode;
	TreeNode* m_pDragNode;
	TreeNode* m_pDropNode;

	const bool m_bUseDummyRoot;
	bool       m_bShowDummy;
	bool       m_bUseFrame;

	bool m_bUseDragDropSelf;
	bool m_bUseDragDropOuter;

	int m_iTest;

	// Clicked Delegate
	UI*     m_pCInst;
	CLICKED m_CFunc;

	// Double Clicked
	UI*     m_pDBCInst;
	CLICKED m_DBCFunc;

	// Drag And Drop
	UI*       m_pDADInst;
	DRAG_DROP m_DADFunc;

	// Key Binding Delegate
	vector<tTreeKey> m_vecKeyBind;

	UI*  m_pDropInstance;
	DROP m_dropFuc;
public:
	TreeNode* GetDragNode() { return m_pDragNode; }
	TreeNode* GetDropNode() { return m_pDropNode; }

	void update() override;
	void render_update() override;

	void DropCheck() override;

public:
	void SetDragDropFunc(UI* _pInstance, DROP func)
	{
		m_pDropInstance = _pInstance;
		m_dropFuc       = func;
	}

	void      ShowDummyRoot(bool _bTrue) { m_bShowDummy = _bTrue; }
	void      UseFrame(bool _b) { m_bUseFrame = _b; }
	void      UseDragDropOuter(bool _b) { m_bUseDragDropOuter = _b; }
	void      UseDragDropSelf(bool _b) { m_bUseDragDropSelf = _b; }
	TreeNode* AddTreeNode(TreeNode* _pParentNode, const string& _strName, DWORD_PTR _dwData = 0);
	TreeNode* GetDummyNode() { return m_pRootNode; }

	void SetClickedDelegate(UI* _pInst, CLICKED _Func)
	{
		m_pCInst = _pInst;
		m_CFunc  = _Func;
	}

	void SetDoubleClickedDelegate(UI* _pInst, CLICKED _Func)
	{
		m_pDBCInst = _pInst;
		m_DBCFunc  = _Func;
	}

	void SetDragAndDropDelegate(UI* _pInst, DRAG_DROP _Func)
	{
		m_pDADInst = _pInst;
		m_DADFunc  = _Func;
	}

	void SetKeyBinding(KEY _eKey, UI* _pInst, KEY_FUNC _Func);

	void Clear();

	TreeNode* FindNode(const std::string& _nodeName);
	void      SetSelectedNode(TreeNode* _pNode);
private:
	void SetDBClickedNode(TreeNode* _pNode);

public:
	TreeUI(bool _bDummyRoot);
	virtual ~TreeUI();

	friend class TreeNode;
};
