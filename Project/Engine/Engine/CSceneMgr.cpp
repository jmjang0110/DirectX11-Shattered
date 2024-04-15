#include "pch.h"
#include "CSceneMgr.h"

#include "CEventMgr.h"
#include "CCollisionMgr.h"

#include "CGameObject.h"
#include "CTransform.h"

#include "CScene.h"
#include "CLayer.h"
#include "CRenderMgr.h"

CSceneMgr::CSceneMgr()
	: m_pCurScene(nullptr) {}

CSceneMgr::~CSceneMgr()
{
	SAFE_DELETE(m_pCurScene);
}

void CSceneMgr::init() {}

void CSceneMgr::progress()
{
	if (nullptr == m_pCurScene)
	{
		return;
	}

	const SCENE_STATE eState = m_pCurScene->GetSceneState();
	if (SCENE_STATE::PLAY == eState)
	{
		m_pCurScene->update();
		m_pCurScene->lateupdate();
	}

	ClearLayer();
	CRenderMgr::GetInst()->ClearCamera();


	m_pCurScene->finalupdate();

	// Collision Check
	if (SCENE_STATE::PLAY == eState)
	{
		CCollisionMgr::GetInst()->update();
	}
}

void CSceneMgr::ChangeScene(CScene* _pNextScene)
{
	if (nullptr != m_pCurScene)
	{
		delete m_pCurScene;
	}
	m_pCurScene = _pNextScene;
}

void CSceneMgr::SpawnObject(CGameObject* _pSpawnObject, Vec3 _vWorldPos, wstring _strName, UINT _iLayerIdx) const
{
	_pSpawnObject->Transform()->SetRelativePos(_vWorldPos);
	_pSpawnObject->SetName(_strName);

	tEventInfo info = {};
	info.eType      = EVENT_TYPE::CREATE_OBJ;
	info.lParam     = reinterpret_cast<DWORD_PTR>(_pSpawnObject);
	info.wParam     = static_cast<DWORD_PTR>(_iLayerIdx);
	CEventMgr::GetInst()->AddEvent(info);
}

void CSceneMgr::SpawnObject(CGameObject* _pSpawnObject, UINT _iLayerIdx) const
{
	tEventInfo info = {};
	info.eType      = EVENT_TYPE::CREATE_OBJ;
	info.lParam     = reinterpret_cast<DWORD_PTR>(_pSpawnObject);
	info.wParam     = static_cast<DWORD_PTR>(_iLayerIdx);
	CEventMgr::GetInst()->AddEvent(info);
}

void CSceneMgr::SpawnObject(CGameObject* _pSpawnObject, wstring _strLayerName) const
{
	const UINT layerIndex = m_pCurScene->GetLayerIdxFromName(_strLayerName);

	tEventInfo info = {};
	info.eType      = EVENT_TYPE::CREATE_OBJ;
	info.lParam     = reinterpret_cast<DWORD_PTR>(_pSpawnObject);
	info.wParam     = static_cast<DWORD_PTR>(layerIndex);
	CEventMgr::GetInst()->AddEvent(info);
}

void CSceneMgr::AddChild(CGameObject* _pParentObject, CGameObject* _pChildObject) const
{
	tEventInfo info = {};
	info.eType      = EVENT_TYPE::ADD_CHILD;
	info.lParam     = reinterpret_cast<DWORD_PTR>(_pParentObject);
	info.wParam     = reinterpret_cast<DWORD_PTR>(_pChildObject);
	CEventMgr::GetInst()->AddEvent(info);
}

void CSceneMgr::DisconnectParent(CGameObject* _pObject)
{
	tEventInfo info = {};
	info.eType      = EVENT_TYPE::DISCONNECT_PARENT;
	info.lParam     = reinterpret_cast<DWORD_PTR>(_pObject);
	CEventMgr::GetInst()->AddEvent(info);
}

void CSceneMgr::ChangeObjectLayerIndex(CGameObject* _pObject, UINT _iLayerIdx) const
{
	tEventInfo info = {};
	info.eType      = EVENT_TYPE::CHANGE_OBJ_LAYER_INDEX;
	info.lParam     = reinterpret_cast<DWORD_PTR>(_pObject);
	info.wParam     = static_cast<DWORD_PTR>(_iLayerIdx);
	CEventMgr::GetInst()->AddEvent(info);
}

void CSceneMgr::SwapLayer(UINT oldLayer, UINT newLayer) const
{
	tEventInfo info = {};
	info.eType      = EVENT_TYPE::SWAP_LAYER;
	info.lParam     = static_cast<DWORD_PTR>(oldLayer);
	info.wParam     = static_cast<DWORD_PTR>(newLayer);
	CEventMgr::GetInst()->AddEvent(info);
}

void CSceneMgr::ClearLayer() const
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_pCurScene->GetLayer(i)->Clear();
	}
}

void CSceneMgr::DeleteObject(CGameObject* _pDestroyObject) const
{
	tEventInfo info = {};
	info.eType      = EVENT_TYPE::DELETE_OBJ;
	info.lParam     = reinterpret_cast<DWORD_PTR>(_pDestroyObject);
	CEventMgr::GetInst()->AddEvent(info);
}

CGameObject* CSceneMgr::FindObjectByName(const wstring& _strName) const
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = m_pCurScene->GetLayer(i);
		for (const auto& pGameObject : pLayer->GetObjects())
		{
			if (_strName == pGameObject->GetName())
			{
				return pGameObject;
			}
		}
	}
	return nullptr;
}

void CSceneMgr::FindObjectsByName(const wstring& _strName, vector<CGameObject*>& _vecOut) const
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = m_pCurScene->GetLayer(i);
		for (auto& pGameObject : pLayer->GetObjects())
		{
			if (_strName == pGameObject->GetName())
			{
				_vecOut.push_back(pGameObject);
			}
		}
	}
}
