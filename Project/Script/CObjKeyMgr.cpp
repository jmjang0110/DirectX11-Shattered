#include "pch.h"
#include "CObjKeyMgr.h"


// 


CObjKeyMgr::CObjKeyMgr()
{
	Init();
}

CObjKeyMgr::CObjKeyMgr(const CObjKeyMgr& _origin)
{
	Init();
}

CObjKeyMgr::~CObjKeyMgr()
{
}

void CObjKeyMgr::Init()
{
	// DEFAULT KEY SETTING

	/// [ 이동 키 ]
	m_tMovingKey.eForward		= KEY::W;
	m_tMovingKey.eBackWard		= KEY::S;
	m_tMovingKey.eLeft			= KEY::A;
	m_tMovingKey.eRight			= KEY::D;

	m_tMovingKey.eJump			= KEY::SPACE;
	m_tMovingKey.eInteract		= KEY::E;
	m_tMovingKey.eSprint		= KEY::LCTRL;
	m_tMovingKey.eWalk			= KEY::V;

	// [ 전투 키 ]
	m_tCombatKey.eLightAttack	= KEY::LBTN;
	m_tCombatKey.eHeavyAttack	= KEY::F;
	m_tCombatKey.eParry			= KEY::Q;
	m_tCombatKey.eCameraLock	= KEY::MBTN;
}


void CObjKeyMgr::update()
{
	m_tPrevKeyInfo = m_tCurKeyInfo;

	m_vecCurKeyZipInfo.clear();
	m_tCurKeyInfo.tKeyFlags_Zip.Clear();

	// 이동 키
	CheckMovingKeyTap();
	CheckMovingKeyPressed();
	CheckMovingKeyAway();

	// 전투 키
	CheckCombatKeyTap();
	CheckCombatKeyPressed();
	checkCombatKeyAway();

}

void CObjKeyMgr::lateupdate()
{
}


void CObjKeyMgr::CheckMovingKeyTap()
{
	// KEY TAP
	if (KEY_TAP(m_tMovingKey.eForward))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::TAP;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::FORWARD;

		m_tCurKeyInfo.eKeyState = KEY_STATE::TAP;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eForward;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_TAP(m_tMovingKey.eBackWard))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::TAP;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::BACKWARD;

		m_tCurKeyInfo.eKeyState = KEY_STATE::TAP;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eBackWard;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_TAP(m_tMovingKey.eLeft))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::TAP;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::LEFT;

		m_tCurKeyInfo.eKeyState = KEY_STATE::TAP;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eLeft;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_TAP(m_tMovingKey.eRight))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::TAP;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::RIGHT;

		m_tCurKeyInfo.eKeyState = KEY_STATE::TAP;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eRight;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}


	if (KEY_TAP(m_tMovingKey.eJump))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::TAP;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::JUMP;

		m_tCurKeyInfo.eKeyState = KEY_STATE::TAP;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eJump;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_TAP(m_tMovingKey.eInteract))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::TAP;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::INTERACT;

		m_tCurKeyInfo.eKeyState = KEY_STATE::TAP;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eInteract;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_TAP(m_tMovingKey.eSprint))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::TAP;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::SPRINT;

		m_tCurKeyInfo.eKeyState = KEY_STATE::TAP;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eSprint;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_TAP(m_tMovingKey.eWalk))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::TAP;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::WALK;

		m_tCurKeyInfo.eKeyState = KEY_STATE::TAP;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eWalk;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
}

void CObjKeyMgr::CheckMovingKeyPressed()
{
	// KEY PRESSED
	if (KEY_PRESSED(m_tMovingKey.eForward))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed |= PLAYER_KEY_OPTION::PRESSED;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed |= PLAYER_KEY_OPTION::FORWARD;

		m_tCurKeyInfo.eKeyState = KEY_STATE::PRESSED;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eForward;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_PRESSED(m_tMovingKey.eBackWard))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed |= PLAYER_KEY_OPTION::PRESSED;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed |= PLAYER_KEY_OPTION::BACKWARD;

		m_tCurKeyInfo.eKeyState = KEY_STATE::PRESSED;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eBackWard;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_PRESSED(m_tMovingKey.eLeft))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed |= PLAYER_KEY_OPTION::PRESSED;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed |= PLAYER_KEY_OPTION::LEFT;

		m_tCurKeyInfo.eKeyState = KEY_STATE::PRESSED;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eLeft;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_PRESSED(m_tMovingKey.eRight))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed |= PLAYER_KEY_OPTION::PRESSED;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed |= PLAYER_KEY_OPTION::RIGHT;

		m_tCurKeyInfo.eKeyState = KEY_STATE::PRESSED;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eRight;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}


	if (KEY_PRESSED(m_tMovingKey.eJump))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed |= PLAYER_KEY_OPTION::PRESSED;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed |= PLAYER_KEY_OPTION::JUMP;

		m_tCurKeyInfo.eKeyState = KEY_STATE::PRESSED;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eJump;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}

	if (KEY_PRESSED(m_tMovingKey.eInteract))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed |= PLAYER_KEY_OPTION::PRESSED;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed |= PLAYER_KEY_OPTION::INTERACT;

		m_tCurKeyInfo.eKeyState = KEY_STATE::PRESSED;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eInteract;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_PRESSED(m_tMovingKey.eSprint))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed |= PLAYER_KEY_OPTION::PRESSED;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed |= PLAYER_KEY_OPTION::SPRINT;

		m_tCurKeyInfo.eKeyState = KEY_STATE::PRESSED;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eSprint;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}

	if (KEY_PRESSED(m_tMovingKey.eWalk))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed |= PLAYER_KEY_OPTION::PRESSED;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Pressed |= PLAYER_KEY_OPTION::WALK;

		m_tCurKeyInfo.eKeyState = KEY_STATE::PRESSED;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eWalk;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
}

void CObjKeyMgr::CheckMovingKeyAway()
{
	// KEY AWAY
	if (KEY_AWAY(m_tMovingKey.eForward))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away |= PLAYER_KEY_OPTION::AWAY;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away |= PLAYER_KEY_OPTION::FORWARD;

		m_tCurKeyInfo.eKeyState = KEY_STATE::AWAY;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eForward;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_AWAY(m_tMovingKey.eBackWard))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away |= PLAYER_KEY_OPTION::AWAY;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away |= PLAYER_KEY_OPTION::BACKWARD;

		m_tCurKeyInfo.eKeyState = KEY_STATE::AWAY;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eBackWard;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_AWAY(m_tMovingKey.eLeft))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away |= PLAYER_KEY_OPTION::AWAY;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away |= PLAYER_KEY_OPTION::LEFT;

		m_tCurKeyInfo.eKeyState = KEY_STATE::AWAY;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eLeft;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_AWAY(m_tMovingKey.eRight))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away |= PLAYER_KEY_OPTION::AWAY;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away |= PLAYER_KEY_OPTION::RIGHT;

		m_tCurKeyInfo.eKeyState = KEY_STATE::AWAY;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eRight;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}


	if (KEY_AWAY(m_tMovingKey.eJump))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away |= PLAYER_KEY_OPTION::AWAY;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away |= PLAYER_KEY_OPTION::JUMP;

		m_tCurKeyInfo.eKeyState = KEY_STATE::AWAY;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eJump;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_AWAY(m_tMovingKey.eInteract))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away |= PLAYER_KEY_OPTION::AWAY;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away |= PLAYER_KEY_OPTION::INTERACT;

		m_tCurKeyInfo.eKeyState = KEY_STATE::AWAY;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eInteract;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_AWAY(m_tMovingKey.eSprint))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away |= PLAYER_KEY_OPTION::AWAY;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away |= PLAYER_KEY_OPTION::SPRINT;

		m_tCurKeyInfo.eKeyState = KEY_STATE::AWAY;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eSprint;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_AWAY(m_tMovingKey.eWalk))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away |= PLAYER_KEY_OPTION::AWAY;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Away |= PLAYER_KEY_OPTION::WALK;

		m_tCurKeyInfo.eKeyState = KEY_STATE::AWAY;
		m_tCurKeyInfo.eKey      = m_tMovingKey.eWalk;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
}

void CObjKeyMgr::CheckCombatKeyTap()
{
	if (KEY_TAP(m_tCombatKey.eLightAttack))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::LIGHT_ATTACK;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::TAP;

		m_tCurKeyInfo.eKeyState = KEY_STATE::TAP;
		m_tCurKeyInfo.eKey = m_tCombatKey.eLightAttack;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_TAP(m_tCombatKey.eHeavyAttack))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::HEAVY_ATTACK;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::TAP;
		
		m_tCurKeyInfo.eKeyState = KEY_STATE::TAP;
		m_tCurKeyInfo.eKey = m_tCombatKey.eHeavyAttack;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_TAP(m_tCombatKey.eParry))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::PARRY;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::TAP;
		
		m_tCurKeyInfo.eKeyState = KEY_STATE::TAP;
		m_tCurKeyInfo.eKey = m_tCombatKey.eParry;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_TAP(m_tCombatKey.eCameraLock))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::CAMERA_LOCK;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::TAP;
		
		m_tCurKeyInfo.eKeyState = KEY_STATE::TAP;
		m_tCurKeyInfo.eKey = m_tCombatKey.eCameraLock;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}

}

void CObjKeyMgr::CheckCombatKeyPressed()
{
	if (KEY_PRESSED(m_tCombatKey.eLightAttack))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::LIGHT_ATTACK;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::PRESSED;

		m_tCurKeyInfo.eKeyState = KEY_STATE::PRESSED;
		m_tCurKeyInfo.eKey = m_tCombatKey.eLightAttack;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_PRESSED(m_tCombatKey.eHeavyAttack))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::HEAVY_ATTACK;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::PRESSED;

		m_tCurKeyInfo.eKeyState = KEY_STATE::PRESSED;
		m_tCurKeyInfo.eKey = m_tCombatKey.eHeavyAttack;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_PRESSED(m_tCombatKey.eParry))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::PARRY;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::PRESSED;

		m_tCurKeyInfo.eKeyState = KEY_STATE::PRESSED;
		m_tCurKeyInfo.eKey = m_tCombatKey.eParry;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_PRESSED(m_tCombatKey.eCameraLock))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::CAMERA_LOCK;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::PRESSED;

		m_tCurKeyInfo.eKeyState = KEY_STATE::PRESSED;
		m_tCurKeyInfo.eKey = m_tCombatKey.eCameraLock;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
}

void CObjKeyMgr::checkCombatKeyAway()
{
	if (KEY_AWAY(m_tCombatKey.eLightAttack))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::LIGHT_ATTACK;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::AWAY;

		m_tCurKeyInfo.eKeyState = KEY_STATE::AWAY;
		m_tCurKeyInfo.eKey = m_tCombatKey.eLightAttack;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_AWAY(m_tCombatKey.eHeavyAttack))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::HEAVY_ATTACK;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::AWAY;

		m_tCurKeyInfo.eKeyState = KEY_STATE::AWAY;
		m_tCurKeyInfo.eKey = m_tCombatKey.eHeavyAttack;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);

	}
	if (KEY_AWAY(m_tCombatKey.eParry))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::PARRY;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::AWAY;

		m_tCurKeyInfo.eKeyState = KEY_STATE::AWAY;
		m_tCurKeyInfo.eKey = m_tCombatKey.eParry;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
	if (KEY_AWAY(m_tCombatKey.eCameraLock))
	{
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::CAMERA_LOCK;
		m_tCurKeyInfo.tKeyFlags_Zip.iKeyFlags_Tap |= PLAYER_KEY_OPTION::AWAY;

		m_tCurKeyInfo.eKeyState = KEY_STATE::AWAY;
		m_tCurKeyInfo.eKey = m_tCombatKey.eCameraLock;

		tKey_Zip tKeyZip(m_tCurKeyInfo.eKeyState, m_tCurKeyInfo.eKey);
		m_vecCurKeyZipInfo.push_back(tKeyZip);
	}
}


KEY_STATE CObjKeyMgr::GetKeyStateByFlags(KeyOptionFlags _Flags)
{
	// 1. Key State 해석 
	KEY_STATE eKeyState = KEY_STATE::NONE;
	eKeyState = (_Flags & PLAYER_KEY_OPTION::TAP) ? eKeyState = KEY_STATE::TAP : eKeyState = eKeyState;
	eKeyState = (_Flags & PLAYER_KEY_OPTION::PRESSED) ? eKeyState = KEY_STATE::PRESSED : eKeyState = eKeyState;
	eKeyState = (_Flags & PLAYER_KEY_OPTION::AWAY) ? eKeyState = KEY_STATE::AWAY : eKeyState = eKeyState;
	return eKeyState;
}

KEY CObjKeyMgr::GetKeyByFlags(KeyOptionFlags _Flags)
{
	// 2. KEY 해석
	KEY eKey = KEY::KEY_END;
	eKey     = (_Flags & PLAYER_KEY_OPTION::FORWARD) ? eKey  = m_tMovingKey.eForward : eKey  = eKey;
	eKey     = (_Flags & PLAYER_KEY_OPTION::BACKWARD) ? eKey = m_tMovingKey.eBackWard : eKey = eKey;
	eKey     = (_Flags & PLAYER_KEY_OPTION::LEFT) ? eKey     = m_tMovingKey.eLeft : eKey     = eKey;
	eKey     = (_Flags & PLAYER_KEY_OPTION::RIGHT) ? eKey    = m_tMovingKey.eRight : eKey    = eKey;

	eKey = (_Flags & PLAYER_KEY_OPTION::JUMP) ? eKey     = m_tMovingKey.eJump : eKey     = eKey;
	eKey = (_Flags & PLAYER_KEY_OPTION::INTERACT) ? eKey = m_tMovingKey.eInteract : eKey = eKey;
	eKey = (_Flags & PLAYER_KEY_OPTION::SPRINT) ? eKey   = m_tMovingKey.eSprint : eKey   = eKey;
	eKey = (_Flags & PLAYER_KEY_OPTION::WALK) ? eKey     = m_tMovingKey.eWalk : eKey     = eKey;

	eKey = (_Flags & PLAYER_KEY_OPTION::LIGHT_ATTACK) ? eKey = m_tCombatKey.eLightAttack : eKey = eKey;
	eKey = (_Flags & PLAYER_KEY_OPTION::HEAVY_ATTACK) ? eKey = m_tCombatKey.eHeavyAttack : eKey = eKey;
	eKey = (_Flags & PLAYER_KEY_OPTION::PARRY) ? eKey        = m_tCombatKey.eParry : eKey       = eKey;
	eKey = (_Flags & PLAYER_KEY_OPTION::CAMERA_LOCK) ? eKey  = m_tCombatKey.eCameraLock : eKey  = eKey;

	return eKey;
}
