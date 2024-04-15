#pragma once
#include <Engine/CState.h>
#include "CObjKeyMgr.h"
class CStateMgr;
class PlayerScript;
class CAnimation3D;


enum class LIGHT_ATTACK_STATE
{
    NONE,

    COMBO_1,
    COMBO_2,
    COMBO_3,
    COMBO_END,


    END,

};

struct LightAttTimer
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

class CPlayerLightAttackState :
    public CState
{
private:
    CGameObject*        m_pWeaponCollider;
    CGameObject*        m_pWeaponTrail;

private:
    PlayerScript*       m_pPlayerScript;
    CStateMgr*          m_pStateMgr;
    tKey_Zip            m_tCurKeyInfo;          // 눌린 키 정보
    tKey_Zip            m_tPrevKeyInfo;


    LIGHT_ATTACK_STATE  m_eLA_State;            // 약공격 콤보 상태 정보 
    LightAttTimer       m_tAttTimer;            // 약공격 콤보 별 타이머
    CAnimation3D*       m_pCurAnim;

    bool                m_bComboAtt_Start;
    bool                m_bCombo1_2_Success;      // Combo 2 로 넘어 갈 수 있는지 체크 
    bool                m_bCombo2_3_Success;      // Combo 3 로 넘어 갈 수 있는지 체크 

    bool                m_bSwordTrail_Play_1;
    bool                m_bSwordTrail_Play_2;
    bool                m_bSwordTrail_Play_3;


public:
    void SetWeaponCollider(CGameObject* _pObj);
    void SetWeaponTrail(CGameObject* _pObj);
    void TrailOn();
    void UpdateWeaponTrail();
    void UpdateWeaponTrailState();


public:
    virtual void Enter();
    virtual void Exit();

    virtual void Update();
    virtual void LateUpdate();

    void UpdateLightAttackState();
    void UpdateLightAttaclAnim();

    void PlayLightAttackAnim(wstring _sName, bool _bRepeat, float _fSpeed = 1.f);

    CLONE(CPlayerLightAttackState)

public:
    CPlayerLightAttackState();
    CPlayerLightAttackState(const CPlayerLightAttackState& _origin);
    virtual ~CPlayerLightAttackState();

};

