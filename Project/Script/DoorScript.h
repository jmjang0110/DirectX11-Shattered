#pragma once
#include <Engine/CScript.h>

class DoorScript : public CScript
{
	float m_time        = 3.5f;
	float m_elapsedTime = 0;
	bool  m_start       = false;
	bool  m_bEntered    = false;
	bool  m_bOpened     = false;

public:
	bool IsDoorOpened() const { return m_bOpened; }
	void SetOpen(bool _bEnable) { m_bOpened = _bEnable; }

public:
	void start() override;
	void update() override;
	void OnCollisionEnter(CGameObject* _OtherObject) override;

public:
	CLONE(DoorScript);
	DoorScript();
	virtual ~DoorScript();
};
