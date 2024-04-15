#pragma once
#include <Engine\CState.h>

class CAnimation3D;

class Jug_Intro :
    public CState
{
private:
    CAnimation3D*   m_pAnim;
    
    float m_fPaperBurnStrength;

public:
    virtual void Init();
    virtual void Enter();
    virtual void Exit();

    virtual void Update();
    virtual void LateUpdate();

    CLONE(Jug_Intro)

public:
    Jug_Intro();
    Jug_Intro(const Jug_Intro& _origin);
    virtual ~Jug_Intro();
};

