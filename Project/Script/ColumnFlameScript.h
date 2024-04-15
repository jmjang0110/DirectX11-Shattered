#pragma once
#include <Engine\CScript.h>

class CGameObject;

class ColumnFlameScript : public CScript
{
private:
	bool  m_fActiveLaser;
	float m_fTimer;
	float m_fFlameSpeed;
	float m_fRotSpeed;
	float m_fDamage;
	Vec4  m_vColor; // 0 ~ 1

	CGameObject* m_pDecal;
	CGameObject* m_pLaser_1;
	CGameObject* m_pLaser_2;
	CGameObject* m_pDistortion_1;
	CGameObject* m_pDistortion_2;


public:
	void SetDamage(float _fDamage) { m_fDamage = _fDamage; }
	void SetFlameSpeed(float _fSpeed) { m_fFlameSpeed = _fSpeed; }
	void SetRotSpeed(float _fSpeed) { m_fRotSpeed = _fSpeed; }
	void SetColor(Vec4 _vColor) { m_vColor = _vColor; }


	float GetDamage() const { return m_fDamage; }
	float GetFlameSpeed() const { return m_fFlameSpeed; }
	float GetRotSpeed() const { return m_fRotSpeed; }
	Vec4  GetColor() const { return m_vColor; }

	void         Init();
	virtual void start();
	virtual void update();
	virtual void lateupdate();
	

	virtual void OnCollisionEnter(CGameObject* _OtherObject) override;
	virtual void OnCollision(CGameObject* _OtherObject) override;
	virtual void OnCollisionExit(CGameObject* _OtherObject) override;

	CLONE(ColumnFlameScript);

public:
	ColumnFlameScript();
	ColumnFlameScript(const ColumnFlameScript& _origin);
	virtual ~ColumnFlameScript();
};
