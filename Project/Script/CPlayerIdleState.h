#pragma once
#include <Engine/CState.h>

class  CPlayerIdleState :
    public CState
{
private:
    class CStateMgr* m_pStateMgr;


public:
    virtual void Enter();
    virtual void Exit();

    virtual void Update();
    virtual void LateUpdate();


    void PlayIdleAnim(wstring _sName, bool _bRepeat);


    CLONE(CPlayerIdleState)
public:
    CPlayerIdleState();
    CPlayerIdleState(const CPlayerIdleState& _origin);
    virtual ~CPlayerIdleState();

};

