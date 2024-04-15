#pragma once
#include <Engine/CState.h>
#include "CObjKeyMgr.h"
#include "CTranslateMgr.h"

// player 위치 관련 함수 정의 
class CTransform;
class CStateMgr;
class PlayerCamScript;
class CPlayerStat;

enum class MOVE_MODE
{
    RUN,
    WALK,
};


class CPlayerMoveState :
    public CState
{
private:
    CTranslateMgr* m_pTranslateMgr;
    CStateMgr* m_pStateMgr;

    tKey_Zip            m_tCurKeyInfo;          // 눌린 키 정보
    bool                m_bKeyUpdateFinish;     // 키 처리가 중복 되지 않기 위해 키 처리가 끝남을 알린다. 

private:
    float               m_fStandardLerpTime;    // 기존 보간시간
    bool                m_bChangeLerpTime;      // 보간시간을 변경 했는지 여부 


    MOVE_MODE           m_eMode;                // 걷기/ 달리기 모드  

    PlayerCamScript* m_pCamScript;
    CPlayerStat* m_pStat;

public:
    virtual void Enter();
    virtual void Exit();

    virtual void Update();
    virtual void LateUpdate();

public:
    void UpdateMoveMode();
    void UpdateRunMode();
    void UpdateWalkMode();

public:

    void UpdateRunAnimLerpTime();
    void PlayMoveAnim(wstring _sName, bool _bRepeat);

public:
    // [ SET PART ]
    void SetCurKeyInfo(tKey_Zip _keyInfo);

    // [ GET PART ]
public:

    CLONE(CPlayerMoveState)
public:
    CPlayerMoveState();
    CPlayerMoveState(const CPlayerMoveState& _origin);
    virtual ~CPlayerMoveState();

};

