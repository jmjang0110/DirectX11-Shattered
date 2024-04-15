#pragma once
#include <Engine/CScript.h>

class CGameObject;
class CTexture;

enum class EffectType
{
	BOSS_HANP_DROP_EFFECT,

	NONE
};

class EffectScript :
	public CScript
{
private:
	CTexture* m_pEffectTexture;
	EffectType	m_eEffectType;
	float		m_fLifeTime;

public:
	void SetEffectLifeTime(float _time) { m_fLifeTime = _time; }

	void SetEffectTexture(CTexture* _tex) { m_pEffectTexture = _tex; }

	void SetEffectType(EffectType _type) { m_eEffectType = _type; }

public:
	virtual void start();
	virtual void update();
	virtual void lateupdate();

public:
	CLONE(EffectScript)

		void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	EffectScript(const EffectScript& _origin);
	EffectScript();
	virtual ~EffectScript();
};

