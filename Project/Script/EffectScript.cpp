#include "pch.h"
#include "EffectScript.h"

#include <Engine/CResMgr.h>
#include <Engine/CMesh.h>

EffectScript::EffectScript()
	: CScript{ (int)SCRIPT_TYPE::EFFECTSCRIPT }
	, m_pEffectTexture(nullptr)
	, m_fLifeTime(0.f)
	, m_eEffectType(EffectType::NONE)
{
	m_fLifeTime = 2.f;
}

EffectScript::EffectScript(const EffectScript& _origin)
	: CScript{ (int)SCRIPT_TYPE::EFFECTSCRIPT }
	, m_pEffectTexture(_origin.m_pEffectTexture)
	, m_fLifeTime(_origin.m_fLifeTime)
	, m_eEffectType(_origin.m_eEffectType)
{
}

EffectScript::~EffectScript()
{
}

void EffectScript::start()
{
	m_eEffectType = EffectType::BOSS_HANP_DROP_EFFECT;

	if (m_eEffectType == EffectType::BOSS_HANP_DROP_EFFECT)
	{

	}
	else if (m_eEffectType == EffectType::NONE)
	{

	}
}

void EffectScript::update()
{

	return;

	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 vRotation = GetOwner()->Transform()->GetRelativeRotation();
	Vec3 vScale = GetOwner()->Transform()->GetRelativeScale();

	if (m_eEffectType == EffectType::BOSS_HANP_DROP_EFFECT)
	{
		m_fLifeTime -= DT;

		if (m_fLifeTime <= 0.f)
		{
			GetOwner()->Destroy();
		}
		else
		{
			vScale.x *= 1.001f;
			vScale.y *= 1.001f;
		}
	}
	else if (m_eEffectType == EffectType::NONE)
	{
		return;
	}

	GetOwner()->Transform()->SetRelativePos(vPos);
	GetOwner()->Transform()->SetRelativeRotation(vRotation);
	GetOwner()->Transform()->SetRelativeScale(vScale);

}

void EffectScript::lateupdate()
{
}

void EffectScript::Serialize(YAML::Emitter& emitter)
{
}

void EffectScript::Deserialize(const YAML::Node& node)
{
}
