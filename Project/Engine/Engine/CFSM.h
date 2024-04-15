#pragma once
#include "CComponent.h"
#include "CState.h"

class CFSM
	: public CComponent
{
private:
	std::map<wstring, CState*> m_mapState;

	CState* m_pCurState;

	wstring m_sPrevStateName;
	wstring m_sNextStateName;

public:
	void DeleteState(const wstring& _eStateType);
	void AddState(const wstring& _stateName, CState* _pState);
	void ChangeState(const wstring& _sStateType);

public:
	// [ GET PART ]
	CState*        GetCurState() const { return m_pCurState; }
	const wstring& GetPrevStateType() const { return m_sPrevStateName; }
	const wstring& GetCurStateType() const { return m_pCurState->GetStateType(); }
	const wstring& GetNextStateType() const { return m_sNextStateName; }

	CState*                      GetState(const wstring& _stateName);
	int                          GetStateCount() const { return static_cast<int>(m_mapState.size()); }
	const map<wstring, CState*>& GetAllStates() { return m_mapState; }

	void SetCurState(CState* _pCurState) { m_pCurState = _pCurState; }
	void SetCurState(const wstring& _wstrName);

public:
	void finalupdate() override;
	void update() override;
	void lateupdate() override;

public:
	CLONE_DISABLE(CFSM);
	CFSM();
	virtual ~CFSM();

	friend class CEventMgr;
};
