#pragma once
#include <Engine\CState.h>

class BossJugCombatMgrScript;

class JugPhase_1 :
    public CState
{
private:
    BossJugCombatMgrScript* m_pCombatMgr;

    UINT m_iAttackPattern;

    bool m_bFirstAttackDone;
    bool m_bSecondAttackDone;
    bool m_bThirdAttackDone;

public:
    // [ GET / SET]
    UINT GetAttackPattern() { return m_iAttackPattern; }

    virtual void Init();
    virtual void Enter();
    virtual void Exit();

    virtual void Update();
    virtual void LateUpdate();


    CLONE(JugPhase_1)
public:
    JugPhase_1();
    JugPhase_1(const JugPhase_1& _origin);
    virtual ~JugPhase_1();
};

