#pragma once
#include <Engine\CState.h>

class CAnimation3D;

class Jug_SpawnHammer :
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


    CLONE(Jug_SpawnHammer)
public:
    Jug_SpawnHammer();
    Jug_SpawnHammer(const Jug_SpawnHammer& _origin);
    virtual ~Jug_SpawnHammer();
};

