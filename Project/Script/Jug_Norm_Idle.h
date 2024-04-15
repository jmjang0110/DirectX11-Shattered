#pragma once
#include <Engine\CState.h>

class CAnimation3D;

class Jug_Norm_Idle :
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

    CLONE(Jug_Norm_Idle)

public:
    Jug_Norm_Idle();
    Jug_Norm_Idle(const Jug_Norm_Idle& _origin);
    virtual ~Jug_Norm_Idle();
};

