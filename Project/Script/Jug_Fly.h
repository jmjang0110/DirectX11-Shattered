#pragma once
#include <Engine\CState.h>

class CAnimation3D;

class Jug_Fly :
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

    CLONE(Jug_Fly)

public:
    Jug_Fly();
    Jug_Fly(const Jug_Fly& _origin);
    virtual ~Jug_Fly();
};

