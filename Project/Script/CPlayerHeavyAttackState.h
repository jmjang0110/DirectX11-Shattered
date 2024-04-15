#pragma once
#include <Engine/CState.h>
#include "CObjKeyMgr.h"


class CStateMgr;
class PlayerScript;
class CAnimation3D;


enum class HEAVY_ATTACK_STATE
{
    NONE,

    COMBO_1,
    COMBO_2,
    COMBO_3,
    COMBO_END,


    END,


};

struct HeavyAttTimer
{
    float   fCombo1_Timer;
    float   fCombo2_Timer;
    float   fCombo3_Timer;

    void Clear()
    {
        fCombo1_Timer = 0.f;
        fCombo2_Timer = 0.f;
        fCombo3_Timer = 0.f;
    }

};

class CPlayerHeavyAttackState :
    public CState
{
private:
    PlayerScript*       m_pPlayerScript;
    CStateMgr*          m_pStateMgr;
    tKey_Zip            m_tCurKeyInfo;          // 눌린 키 정보
    tKey_Zip            m_tPrevKeyInfo;


    HEAVY_ATTACK_STATE  m_eHA_State;            // 강 공격 콤보 상태 정보 
    HeavyAttTimer       m_tAttTimer;            // 강 공격 콤보 별 타이머
    CAnimation3D*       m_pCurAnim;

    bool                m_bComboAtt_Start;
    bool                m_bCombo1_2_Success;      // Combo 1 -> 2  
    bool                m_bCombo2_3_Success;      // Combo 2 -> 3


public:
    virtual void Enter();
    virtual void Exit();

    virtual void Update();
    virtual void LateUpdate();


    void UpdateHeavyAttackState();
    void UpdateHeavyAttackAnim();

    void PlayHeavyAttackAnim(wstring _sName, bool _bRepeat, float _fSpeed = 1.f);

    CLONE(CPlayerHeavyAttackState)
public:
    CPlayerHeavyAttackState();
    CPlayerHeavyAttackState(const CPlayerHeavyAttackState& _origin);
    virtual ~CPlayerHeavyAttackState();

};

