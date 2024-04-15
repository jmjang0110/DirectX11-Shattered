#pragma once
#include <Engine\CState.h>

class CAnimation3D;

class Jug_Norm_Hit :
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

    CLONE(Jug_Norm_Hit)
public:
    Jug_Norm_Hit();
    Jug_Norm_Hit(const Jug_Norm_Hit& _origin);
    virtual ~Jug_Norm_Hit();
};

