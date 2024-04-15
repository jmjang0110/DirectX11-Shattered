#pragma once
#include <Engine\CState.h>

class CAnimation3D;

class Jug_Attack_0 :
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


    CLONE(Jug_Attack_0)
public:
    Jug_Attack_0();
    Jug_Attack_0(const Jug_Attack_0& _origin);
    virtual ~Jug_Attack_0();
};

