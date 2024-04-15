#pragma once
#include <Engine\CState.h>

class CAnimation3D;

class Jug_WalkGround :
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


    CLONE(Jug_WalkGround)
public:
    Jug_WalkGround();
    Jug_WalkGround(const Jug_WalkGround& _origin);
    virtual ~Jug_WalkGround();
};

