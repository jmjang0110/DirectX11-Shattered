#pragma once
#include <Engine/CState.h>
class CAnimation3D;
class PaperBurnScript;

enum class DIE_STATE
{
    DIE_START,
    DIE_END,
    END,

};

class CPlayerDieState :
    public CState
{
private:
    DIE_STATE m_eState;
    CAnimation3D* m_pCurAnim;

    PaperBurnScript* m_pPaperBurn;

    bool m_bPaperBurnStart;
    bool m_bPaperBurnStart2;


public:
    virtual void Enter();
    virtual void Exit();

    virtual void Update();
    virtual void LateUpdate();
    void PlayDieAnim(wstring _sName, bool _bRepeat, float _fSpeed = 1.f);


    CLONE(CPlayerDieState)
public:
    CPlayerDieState();
    CPlayerDieState(const CPlayerDieState& _origin);
    virtual ~CPlayerDieState();
};

