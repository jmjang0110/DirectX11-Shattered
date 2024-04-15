#pragma once
#include <Engine\CState.h>

class CAnimation3D;

class Jug_Dead :
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

    CLONE(Jug_Dead)

public:
    Jug_Dead();
    Jug_Dead(const Jug_Dead& _origin);
    virtual ~Jug_Dead();
};

