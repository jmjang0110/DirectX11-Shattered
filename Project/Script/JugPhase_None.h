#pragma once
#include <Engine\CState.h>
class JugPhase_None :
    public CState
{
public:
    virtual void Enter();
    virtual void Exit();

    virtual void Update();
    virtual void LateUpdate();


    CLONE(JugPhase_None)
public:
    JugPhase_None();
    JugPhase_None(const JugPhase_None& _origin);
    virtual ~JugPhase_None();
};

