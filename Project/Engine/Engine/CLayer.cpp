#include "pch.h"
#include "CLayer.h"

#include "CGameObject.h"

namespace LAYER
{
	constexpr int INVALID_INDEX = -1;
}

CLayer::CLayer()
	: m_iLayerIdx(LAYER::INVALID_INDEX) {}

CLayer::~CLayer()
{
	Safe_Del_Vec(m_vecRoot);
}

void CLayer::start()
{
	for (const auto& pRootObject : m_vecRoot)
	{
		pRootObject->start();
	}
}

void CLayer::update()
{
	for (const auto& pRootObject : m_vecRoot)
	{
		if (pRootObject->IsActive())
		{
			pRootObject->update();
		}
	}
}

void CLayer::lateupdate()
{
	for (const auto& pRootObject : m_vecRoot)
	{
		if (pRootObject->IsActive())
		{
			pRootObject->lateupdate();
		}
	}
}

void CLayer::finalupdate()
{
	for (auto iter = m_vecRoot.begin(); iter != m_vecRoot.end();)
	{
		CGameObject* pGameObject = *iter;
		pGameObject->finalupdate();

		if (pGameObject->IsDead())
		{
			iter = m_vecRoot.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CLayer::DeregisterObject(CGameObject* _pGameObject)
{
	if (_pGameObject->GetParent())
	{
		_pGameObject->DisconnectBetweenParent();
		ClearObjectLayerIndex(_pGameObject);
	}

	const auto iter = std::find(m_vecRoot.begin(), m_vecRoot.end(), _pGameObject);
	if (iter != m_vecRoot.end())
	{
		m_vecRoot.erase(iter);
		ClearObjectLayerIndex(_pGameObject); // Layer Index = -1;
	}
}

CGameObject* CLayer::FindRootObject(wstring _wstrNames) const
{
	const auto iter = std::find_if(m_vecRoot.begin(),
	                               m_vecRoot.end(),
	                               [&_wstrNames](const CGameObject* pGameObject)
	                               {
		                               return pGameObject->GetName() == _wstrNames;
	                               });
	if (iter != m_vecRoot.end())
	{
		return *iter;
	}
	return nullptr;
}

void CLayer::RenewLayerIdx(int _iLayerIndex)
{
	assert(0<=_iLayerIndex && _iLayerIndex<MAX_LAYER);

	m_iLayerIdx = _iLayerIndex;
	for (const auto& pRootObject : m_vecRoot)
	{
		pRootObject->m_iLayerIdx             = _iLayerIndex;
		const vector<CGameObject*>& vecChild = pRootObject->GetChild();
		for (const auto& pChild : vecChild)
		{
			pChild->RenewLayerIndex(_iLayerIndex);
		}
	}
}

void CLayer::ClearObjectLayerIndex(CGameObject* _pObject)
{
	_pObject->m_iLayerIdx                = LAYER::INVALID_INDEX;
	const vector<CGameObject*>& vecChild = _pObject->GetChild();
	for (const auto& pChild : vecChild)
	{
		ClearObjectLayerIndex(pChild);
	}
}
