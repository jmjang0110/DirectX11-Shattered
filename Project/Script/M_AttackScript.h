#pragma once

#include <Engine/CScript.h>

class M_AttackScript :
    public CScript
{
private:

public:

	virtual void OnCollisionEnter(CGameObject* _OtherObject) override;
	virtual void OnCollision(CGameObject* _OtherObject) override;
	virtual void OnCollisionExit(CGameObject* _OtherObject) override;

	CLONE(M_AttackScript);

public:
	M_AttackScript();
	virtual ~M_AttackScript();
};

