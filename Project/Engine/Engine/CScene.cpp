#include "pch.h"
#include "CScene.h"

#include "CLayer.h"
#include "CGameObject.h"
#include "CRenderMgr.h"
#include "CResMgr.h"
#include "CSceneFile.h"

CScene::CScene()
	: m_arrLayer{}
	, m_eSceneState(SCENE_STATE::STOP)
{
	for (size_t layerIndex = 0; layerIndex < std::size(m_arrLayer); ++layerIndex)
	{
		m_arrLayer[layerIndex]              = new CLayer;
		m_arrLayer[layerIndex]->m_iLayerIdx = static_cast<int>(layerIndex);
	}
}

CScene::~CScene()
{
	Safe_Del_Arr(m_arrLayer);
}


void CScene::start()
{
	for (size_t i = 0; i < std::size(m_arrLayer); ++i)
	{
		m_arrLayer[i]->start();
	}
}

void CScene::update()
{
	for (size_t i = 0; i < std::size(m_arrLayer); ++i)
	{
		m_arrLayer[i]->update();
	}
}

void CScene::lateupdate()
{
	for (size_t i = 0; i < std::size(m_arrLayer); ++i)
	{
		m_arrLayer[i]->lateupdate();
	}
}

void CScene::finalupdate()
{
	for (size_t i = 0; i < std::size(m_arrLayer); ++i)
	{
		m_arrLayer[i]->finalupdate();
	}
}


void CScene::SetLayerName(int _iLayerIdx, const wstring& _strName) const
{
	assert(0 <= _iLayerIdx && _iLayerIdx < MAX_LAYER);
	GetLayer(_iLayerIdx)->SetName(_strName);
}


int CScene::GetLayerIdxFromName(const wstring& _strName) const
{
	for (int layerIndex = 0; layerIndex < MAX_LAYER; ++layerIndex)
	{
		if (m_arrLayer[layerIndex]->GetName() == _strName)
		{
			return layerIndex;
		}
	}

	return -1;
}

wstring CScene::GetLayerNameFromIdx(int _iLayerIdx) const
{
	assert(0 <= _iLayerIdx && _iLayerIdx < MAX_LAYER);
	wstring LayerName = m_arrLayer[_iLayerIdx]->GetName();
	return LayerName;

}


void CScene::AddObject(CGameObject* _pRootObj, const wstring& _strLayerName) const
{
	const int iLayerIndex = GetLayerIdxFromName(_strLayerName);
	assert(iLayerIndex != -1);
	assert(nullptr == _pRootObj->m_pParent);
	AddObject(_pRootObj, iLayerIndex);
}

void CScene::AddObject(CGameObject* _pRootObj, int _iLayerIdx) const
{
	assert(0 <= _iLayerIdx && _iLayerIdx < MAX_LAYER);
	assert(nullptr == _pRootObj->m_pParent);

	m_arrLayer[_iLayerIdx]->RegisterObjectAsRoot(_pRootObj);

	// 자식오브젝트들도 해당 레이어의 인덱스를 알려준다.
	list<CGameObject*> queue{};
	queue.push_back(_pRootObj);
	// 부모 오브젝트 포함, 자식들 모두 해당 레이어의 인덱스를 알려준다(특정 레이어 소속이 아닌경우에)
	while (false == queue.empty())
	{
		CGameObject* pTargetObj = queue.front();
		queue.pop_front();

		if (-1 == pTargetObj->m_iLayerIdx)
		{
			pTargetObj->m_iLayerIdx = _iLayerIdx;
		}
		const vector<CGameObject*>& vecChild = pTargetObj->GetChild();
		for (const auto& pChild : vecChild)
		{
			queue.push_back(pChild);
		}
	}
}

CLayer* CScene::GetLayer(const wstring& _strLayerName) const
{
	const auto iter = std::find_if(std::begin(m_arrLayer),
	                               std::end(m_arrLayer),
	                               [_strLayerName](const CLayer* pLayer)
	                               {
		                               return pLayer->GetName() == _strLayerName;
	                               });

	if (iter != std::end(m_arrLayer))
	{
		return m_arrLayer[std::distance(std::begin(m_arrLayer), iter)];
	}
	return nullptr;
}

CLayer* CScene::GetLayer(int _iLayerIndex) const
{
	assert(0<=_iLayerIndex && _iLayerIndex<MAX_LAYER);
	return m_arrLayer[_iLayerIndex];
}

Ptr<CSceneFile> CScene::GetSceneFile() const
{
	return CResMgr::GetInst()->FindRes<CSceneFile>(m_strResKey);
}

void CScene::SetSceneState(SCENE_STATE _eState)
{
	if (m_eSceneState == _eState)
	{
		return;
	}

	if (SCENE_STATE::STOP == m_eSceneState)
	{
		if (SCENE_STATE::PLAY == _eState)
		{
			m_eSceneState = _eState;
			start();
		}
	}
	else if (SCENE_STATE::PLAY == m_eSceneState)
	{
		if (SCENE_STATE::PAUSE == _eState)
		{
			m_eSceneState = _eState;
		}
		else if (SCENE_STATE::STOP == _eState)
		{
			m_eSceneState = _eState;
			assert(CResMgr::GetInst()->FindRes<CSceneFile>(m_strResKey).Get());
		}
	}
}


bool CScene::SwapLayer(int _LayerIdx_1, int _LayerIdx_2)
{
	CLayer* Left  = GetLayer(_LayerIdx_1);
	CLayer* Right = GetLayer(_LayerIdx_2);

	CLayer* Empty           = nullptr;
	Empty                   = Left;
	m_arrLayer[_LayerIdx_1] = Right;
	m_arrLayer[_LayerIdx_2] = Empty;

	Left->RenewLayerIdx(_LayerIdx_2);
	Right->RenewLayerIdx(_LayerIdx_1);

	return true;
}
