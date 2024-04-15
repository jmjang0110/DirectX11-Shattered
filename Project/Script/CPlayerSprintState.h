#pragma once
#include <Engine/CState.h>
#include "CStateMgr.h"
#include "CTranslateMgr.h"

class CAnimation3D;

enum class SPRINT_TYPE
{
    JUMP_DASH_PREPARE,
    JUMP_DASH_ING,
    JUMP_DASH_END,

    RUN_DASH,
    END,

};
class CPlayerSprintState :
    public CState
{
private:
    CStateMgr*      m_pStateMgr;
    CTranslateMgr*  m_pTranslateMgr;

    
    wstring         m_sPrevState;
    CAnimation3D*   m_pCurAnim;

    SPRINT_TYPE     m_eSprintType;


public:
    virtual void Enter();
    virtual void Exit();

    virtual void Update();
    virtual void LateUpdate();

    void PlaySprintAnim(wstring _sName, bool _bRepeat = true, float _fSpeed = 1.f);
    void Sprint(float _fForce); // 스프린트 한다.
    void GravityIgnore(bool _b);


    void UpdateSprintState();
    void UpdateSprintAnim();

    CLONE(CPlayerSprintState)

public:
    CPlayerSprintState();
    CPlayerSprintState(const CPlayerSprintState& _origin);
    virtual ~CPlayerSprintState();

};
