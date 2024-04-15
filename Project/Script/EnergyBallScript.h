#pragma once
#include <Engine\CScript.h>

enum class ENERGYBALL_MODE
{
	NONE,		// 제자리에 그대로 있음
	ROTATION,	// 중심점을 두고 회전함
	MISSILE		// 타겟을 향해 날아가 폭발함 
};

enum class ROT_DIR
{
	VERTICAL,
	HORIZONTAL
};

class EnergyBallScript : public CScript
{
private:
	ENERGYBALL_MODE m_eCurMode;
	ROT_DIR         m_eRotDir;
	Vec3            m_vTargetPos;
	Vec3            m_vDir;
	float           m_fRadius;
	float           m_fSpeed;
	float           m_fTimeLimit;
	float           m_fAngle;
	float           m_fTimer;
	bool            m_bFinish;
	bool            m_bMoveTargetPos;

	void Explode();

public:
	void            SetCurMode(ENERGYBALL_MODE _eMode) { m_eCurMode = _eMode; }
	void            SetRotDir(ROT_DIR _eRotDir) { m_eRotDir = _eRotDir; }
	void            SetTargetPos(Vec3 _vPos);
	void            SetRadius(float _fRadius) { m_fRadius = _fRadius; }
	void            SetSpeed(float _fSpeed) { m_fSpeed = _fSpeed; }
	ENERGYBALL_MODE GetCurMode() { return m_eCurMode; }
	Vec3            GetTargetPos() { return m_vTargetPos; }
	float           GetRadius() { return m_fRadius; }
	float           GetSpeed() { return m_fSpeed; }
	ROT_DIR         GetRotDir() { return m_eRotDir; }

	bool IsFinish() { return m_bFinish; }

	void Start(ENERGYBALL_MODE _eMode, Vec3 _vTargetPos = Vec3(0.f, 0.f, 0.f), float _fTimeLimit = 0.f)
	{
		m_eCurMode = _eMode;
		SetTargetPos(_vTargetPos);
		m_fTimeLimit = _fTimeLimit;
		m_bFinish    = false;
	}

	virtual void start();
	virtual void update();
	virtual void lateupdate();

	virtual void OnCollisionEnter(CGameObject* _OtherObject) override;
	virtual void OnCollision(CGameObject* _OtherObject) override;
	virtual void OnCollisionExit(CGameObject* _OtherObject) override;

public:
	virtual void SaveToScene(FILE* _pFile) override;
	virtual void LoadFromScene(FILE* _pFile) override;

	CLONE(EnergyBallScript)

	EnergyBallScript();
	EnergyBallScript(const EnergyBallScript& _origin);
	~EnergyBallScript();
};
