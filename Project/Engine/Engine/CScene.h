#pragma once
#include "CEntity.h"

#include "CSceneFile.h"

class CLayer;
class CGameObject;

class CScene
	: public CEntity
{
private:
	CLayer*     m_arrLayer[MAX_LAYER];
	SCENE_STATE m_eSceneState;
	wstring     m_strResKey;

public:
	void start();
	void update();
	void lateupdate();
	void finalupdate();

public:
	void AddObject(CGameObject* _pRootObj, const wstring& _strLayerName) const;
	void AddObject(CGameObject* _pRootObj, int _iLayerIdx) const;

	bool    SwapLayer(int _LayerIdx_1, int _LayerIdx_2);
	void    SetLayerName(int _iLayerIdx, const wstring& _strName) const;
	int     GetLayerIdxFromName(const wstring& _strName) const;
	wstring GetLayerNameFromIdx(int _iLayerIdx) const;
	CLayer* GetLayer(const wstring& _strLayerName) const;
	CLayer* GetLayer(int _iLayerIndex) const;

	void            SetResKey(const wstring& _strKey) { m_strResKey = _strKey; }
	Ptr<CSceneFile> GetSceneFile() const;

	SCENE_STATE GetSceneState() const { return m_eSceneState; }
	void        SetSceneState(SCENE_STATE _eState);

public:
	CLONE(CScene)
	CScene();
	virtual ~CScene();
};
