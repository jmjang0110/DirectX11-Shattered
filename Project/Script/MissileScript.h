#pragma once
#include <Engine/CScript.h>

class MissileScript : public CScript
{
private:
	float m_fSpeed;
	float m_fAccTime;

public:
	virtual void update();

	virtual void OnCollisionEnter(CGameObject* _pOtherObj) override;

	CLONE(MissileScript);
public:
	MissileScript();
	virtual ~MissileScript();
};
