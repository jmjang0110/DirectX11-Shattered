#pragma once
#include <Engine\CScript.h>

class HammerScript : public CScript
{
private:
	Vec3 m_vOffsetPos;

public:
	void SetHammerColliderOffsetPos(Vec3 _vPos) { m_vOffsetPos = _vPos; }

	virtual void start();
	virtual void update();
	virtual void lateupdate();

	virtual void OnCollisionEnter(CGameObject* _OtherObject) override;
	virtual void OnCollision(CGameObject* _OtherObject) override;
	virtual void OnCollisionExit(CGameObject* _OtherObject) override;

	CLONE(HammerScript)

	HammerScript();
	HammerScript(const HammerScript& _origin);
	virtual ~HammerScript();
};
