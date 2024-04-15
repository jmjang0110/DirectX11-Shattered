#pragma once
#include <Engine/CState.h>
class CAnimation3D;
class PaperBurnScript;

enum class HIT_STATE
{
    HIT_SUCCESS,
    HIT_FAIL,
    END,

};

class CPlayerHitState :
    public CState
{
private:
    HIT_STATE           m_eState;
    CAnimation3D*       m_pCurAnim;
    PaperBurnScript*    m_pPaperBurn;


public:
    virtual void Enter();
    virtual void Exit();

    virtual void Update();
    virtual void LateUpdate();

    void PlayHitAnim(wstring _sName, bool _bRepeat, float _fSpeed = 1.f);

    CLONE(CPlayerHitState)
public:
    CPlayerHitState();
    CPlayerHitState(const CPlayerHitState& _origin);
    virtual ~CPlayerHitState();
};

