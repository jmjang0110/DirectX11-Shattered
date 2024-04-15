#pragma once
#include <Engine/CState.h>
#include "CObjKeyMgr.h"
#include "CTranslateMgr.h"

class CStateMgr;

// JUMP 
// Up -> Air -> Down 
enum class JUMP_STATE
{
    NONE,

    FIRST_JUMP,     // 첫번째 점프
    SECOND_JUMP,    // 두번쨰 점프 (더블 점프)
    LIMIT_JUMP,     // 점프 횟수 제한 

    JUMP_UP,
    JUMP_AIR,
    JUMP_DOWN,

    END,
};

class CPlayerJumpState :
    public CState
{
private:
    CTranslateMgr*  m_pTranslateMgr; // 이동 관련 매니저
    CStateMgr*      m_pStateMgr;


private:
    // [ 점프 관련 변수 ]
    bool             m_bJumpAir_AnimStart;
    bool             m_bJumpDown_AnimStart;

    float m_fPrevHeight;          // 플레이어의 이전 y 값 
    float m_fCurHeight;           // 플레이어의 현재 y 값 
    float m_fCurHeightDir;

    JUMP_STATE       m_ePrevState;
    JUMP_STATE       m_eCurState;

    tKey_Zip         m_tPrevKeyInfo;          // 눌린 키 정보
    tKey_Zip         m_tCurKeyInfo;          // 눌린 키 정보


    JUMP_STATE       m_eJumpState;
    int              m_iJumpCnt;        // 점프 횟수  

    CGameObject*    m_pJumpEffect_Land;



public:
    virtual void Enter();
    virtual void Exit();

    virtual void Update();
    virtual void LateUpdate();

    void  PlayJumpAnim(wstring _sName, bool _bRepeat = true, float _fSpeed = 1.f);
    void  SetCurKeyInfo(tKey_Zip _keyInfo) { m_tCurKeyInfo = _keyInfo; }
    float GetHeightDir() { return m_fCurHeightDir; }
public:
    void Jump(Vec3 _velocity);
    void ResetForDoubleJump();

    void UpdateJump();
    void UpdateJumpState();
    void UpdateJumpAnimation();


    CLONE(CPlayerJumpState)
public:
    CPlayerJumpState();
    CPlayerJumpState(const CPlayerJumpState& _origin);
    virtual ~CPlayerJumpState();

};

