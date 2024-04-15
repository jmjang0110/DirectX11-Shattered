#pragma once
#include <Engine\CState.h>

class CAnimation3D;

class Jug_Hammer_Hit :
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

    CLONE(Jug_Hammer_Hit)

public:
    Jug_Hammer_Hit();
    Jug_Hammer_Hit(const Jug_Hammer_Hit& _origin);
    virtual ~Jug_Hammer_Hit();
};

