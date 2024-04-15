#pragma once

class CScene;
class CGameObject;

class CSceneMgr
	: public CSingleton<CSceneMgr>
{
private:
	SINGLE(CSceneMgr);

private:
	CScene* m_pCurScene;

public:
	CScene* GetCurScene() const { return m_pCurScene; }

	void ChangeScene(CScene* _pNextScene);

	void SpawnObject(CGameObject* _pSpawnObject, Vec3 _vWorldPos, wstring _strName, UINT _iLayerIdx) const;
	void SpawnObject(CGameObject* _pSpawnObject, UINT _iLayerIdx) const;
	void SpawnObject(CGameObject* _pSpawnObject, wstring _strLayerName) const;

	// DESTROY 
	void DeleteObject(CGameObject* _pDestroyObject) const;

	void AddChild(CGameObject* _pParentObject, CGameObject* _pChildObject) const;
	void DisconnectParent(CGameObject* _pObject);

	void ClearLayer() const;
	void ChangeObjectLayerIndex(CGameObject* _pObject, UINT _iLayerIdx) const;
	void SwapLayer(UINT oldLayer, UINT newLayer) const;

	CGameObject* FindObjectByName(const wstring& _strName) const;
	void         FindObjectsByName(const wstring& _strName, vector<CGameObject*>& _vecOut) const;

public:
	void init();
	void progress();
};
