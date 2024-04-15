#pragma once
#include   <Engine\CState.h>

class CAnimation3D;

class Jug_None :
    public CState
{
private:
    CAnimation3D*   m_pAnim;

public:
    virtual void Enter();
    virtual void Exit();

    virtual void Update();
    virtual void LateUpdate();


    CLONE(Jug_None)
public:
    Jug_None();
    Jug_None(const Jug_None& _origin);
    virtual ~Jug_None();
};

