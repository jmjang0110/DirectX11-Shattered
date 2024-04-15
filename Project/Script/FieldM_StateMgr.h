#pragma once
#include <Engine/CEntity.h>

class FieldM_StateMgr :
	public CEntity
{
private:
	CGameObject* m_pOwnerObj;

	wstring					m_sPrevState;
	wstring					m_sCurState;
	wstring					m_sNextState;

	float					m_fRunTime;

public:
	void	update();
	void	lateUpdate();

public:
	void SetRunTime(float _time) { m_fRunTime = _time; }
	float GetRunTime() { return m_fRunTime; }
	void NotUseRunTume() { m_fRunTime = -1.f; }
	bool IsRunTimeEXIST() {
		if (-1.f != m_fRunTime)
			return true;
		else
			return false;
	}

	wstring GetCurState() { return m_sCurState; }

public:
	void InitState(CGameObject* _pOwner);
	void SetNextState(wstring _state) { m_sNextState = _state; }
	CLONE(FieldM_StateMgr)

public:
	void ChangeState(wstring _sStateName);

public:
	FieldM_StateMgr();
	virtual ~FieldM_StateMgr();
};

