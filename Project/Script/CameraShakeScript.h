#pragma once
#include <Engine\CScript.h>

class CameraShakeScript : public CScript
{
private:
	bool  m_bShake;
	int   m_iShaker;
	float m_fEffectTime;
	float m_fTimer;
	float m_fTimeSpace;

public:
	void update() override;

	void StartShake(float _fTime);
	bool IsShake() const { return m_bShake; }

	CLONE(CameraShakeScript)
	CameraShakeScript();
	virtual ~CameraShakeScript();
};
