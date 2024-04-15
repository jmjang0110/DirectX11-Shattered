#pragma once
#include <Engine\CState.h>

class CAnimation3D;

class Jug_Hammer_Idle :
    public CState
{
private:
    CAnimation3D* m_pAnim;

public:
    virtual void Init();
    virtual void Enter();
    virtual void Exit();

    virtual void Update();
    virtual void LateUpdate();

    CLONE(Jug_Hammer_Idle)

public:
    Jug_Hammer_Idle();
    Jug_Hammer_Idle(const Jug_Hammer_Idle& _origin);
    virtual ~Jug_Hammer_Idle();
};

