#pragma once
#include <Engine/CState.h>
#include "CObjKeyMgr.h"

class PlayerScript;
class CStateMgr;
class CAnimation3D;



enum class PARRY_STATE
{
    NONE,

    TRY,
    FAIL,
    SUCCESS,


    END,
};

class CPlayerParryState :
    public CState
{
private:
    PlayerScript*       m_pPlayerScript;
    CStateMgr*          m_pStateMgr;
    tKey_Zip            m_tCurKeyInfo;          // 눌린 키 정보
    tKey_Zip            m_tPrevKeyInfo;


    PARRY_STATE         m_eParryState;
    CAnimation3D*       m_pCurAnim;

    bool                m_bParrySuccess;        // 패리 성공 여부 

public:
    virtual void Enter();
    virtual void Exit();

    virtual void Update();
    virtual void LateUpdate();

    void UpdateParryState();
    void PlayParryAnim(wstring _sName, bool _bRepeat, float _fSpeed);


    CLONE(CPlayerParryState)
public:
    CPlayerParryState();
    CPlayerParryState(const CPlayerParryState& _origin);
    virtual ~CPlayerParryState();
};

