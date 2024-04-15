#include "pch.h"
#include "CFSM.h"
#include "CEventMgr.h"

CFSM::CFSM()
	: CComponent{COMPONENT_TYPE::FSM}
	, m_pCurState(nullptr) {}

CFSM::~CFSM()
{
	Safe_Del_Map(m_mapState);
}

void CFSM::update()
{
	if (m_pCurState != nullptr)
	{
		m_pCurState->Update();
	}
}

void CFSM::lateupdate()
{
	if (m_pCurState != nullptr)
	{
		m_pCurState->LateUpdate();
	}
}

void CFSM::SetCurState(const wstring& _wstrName)
{
	CState* pState = GetState(_wstrName);
	assert(pState);
	m_pCurState = pState;
}

void CFSM::finalupdate() {}

CState* CFSM::GetState(const wstring& _stateName)
{
	const auto iter = m_mapState.find(_stateName);
	if (iter != m_mapState.end())
	{
		return iter->second;
	}
	return nullptr;
}

void CFSM::AddState(const wstring& _stateName, CState* _pState)
{
	const auto iter = m_mapState.find(_stateName);
	if (iter != m_mapState.end())
	{
		return;
	}
	_pState->SetName(_stateName);
	_pState->SetOwner(GetOwner());
	m_mapState.insert({_stateName, _pState});
}

void CFSM::ChangeState(const wstring& _sStateType)
{
	auto iter = m_mapState.find(_sStateType);
	LOG_ASSERT(iter!=m_mapState.end(), "INVALID STATE");

	tEventInfo info{};
	info.eType  = EVENT_TYPE::CHANGE_FSM_STATE;
	info.lParam = (DWORD_PTR)this;
	info.wParam = (DWORD_PTR)iter->second;

	CEventMgr::GetInst()->AddEvent(info);
}


void CFSM::DeleteState(const wstring& _strStateName)
{
	const CState* pState = GetState(_strStateName);
	LOG_ASSERT(pState, "DELETING INVALID STATE");
	SAFE_DELETE(pState);
	m_mapState.erase(_strStateName);
}
