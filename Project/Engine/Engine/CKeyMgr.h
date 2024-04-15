#pragma once

enum class KEY
{
	Q, W, E, R, T, Y, U, I, O, P,
	A, S, D, F, G, H, J, K, L,
	Z, X, C, V, B, N, M,
	LEFT, RIGHT, UP, DOWN,
	_0, _1, _2, _3, _4, _5, _6, _7, _8, _9,
	NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9,
	LSHFT, LALT, LCTRL, SPACE, ENTER, BACK, ESC, TAB, DEL,
	LBTN, RBTN, MBTN,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

	KEY_END,
};

enum class KEY_STATE
{
	TAP,     // 막 눌림
	PRESSED, // TAP 이후에도 계속 눌려있음
	AWAY,    // 막 뗀 상황
	NONE,    // 안눌린 상태
};


struct tKeyInfo
{
	KEY_STATE eState;     // 현재 키 상태
	bool      bPrevCheck; // 이전 프레임에서 키가 눌렸는지 체크
};

class CKeyMgr
	: public CSingleton<CKeyMgr>
{
private:
	SINGLE(CKeyMgr);

private:
	vector<tKeyInfo> m_vecKey;
	Vec2             m_vMousePos;
	Vec2             m_vMousePrevPos;
	Vec2             m_vMouseDir;

	std::bitset<(UINT)KEY::KEY_END> m_arrEnabled;

public:
	void init();
	void update();

	void SetEnableInput(KEY _key, bool _enable);
	void EnableAll(bool _enable);

public:
	KEY_STATE GetKeyState(KEY _eKey) const
	{
		if (m_arrEnabled[(UINT)_eKey])
		{
			return m_vecKey[(UINT)_eKey].eState;
		}
		return KEY_STATE::NONE;
	}

	Vec2 GetMousePos() const { return m_vMousePos; }
	Vec2 GetMouseDir() const { return m_vMouseDir; }

	Vec2 GetMouseDelta() const
	{
		Vec2 vDelta = Vec2(m_vMousePos.x - m_vMousePrevPos.x, m_vMousePos.y - m_vMousePrevPos.y);
		return vDelta;
	}
};
