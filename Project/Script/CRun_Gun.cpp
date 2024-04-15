#include "pch.h"
#include "CRun_Gun.h"

#include <Engine/CAnimator3D.h>
#include <Engine/CGameObject.h>

#include "MonsterGunScript.h"
#include <Engine/CFSM.h>

CRun_Gun::CRun_Gun(MonsterGunScript* pScript)
	: CState{L"RUN"}
	, m_pScript{pScript}
	, m_runSpeed{400.f}
	, m_rotationSpeed{45.f}
{
}

CRun_Gun::~CRun_Gun()
{
}


void CRun_Gun::Update()
{
	CGameObject* pTarget = m_pScript->GetSearchScript().GetTarget();
	if (nullptr != pTarget)
	{
		const Vec3 monsterPos = GetOwner()->Transform()->GetWorldPos();
		const Vec3 playerPos  = pTarget->Transform()->GetWorldPos();

		const Vec3  direction           = Vec3{playerPos - monsterPos};
		const float distanceSquared     = direction.LengthSquared();
		const float prepareRangeSquared = std::powf(m_pScript->GetPrepareRange(), 2.f);

		if (distanceSquared < prepareRangeSquared)
		{
			GetOwner()->FSM()->ChangeState(GAME::DEUXIEMIE::ATTACK);
		}
		else
		{
			const Vec3  monsterFront = -GetOwner()->Transform()->GetWorldFrontDir();
			const float cos          = monsterFront.Dot(Vec3{direction.x, monsterFront.y, direction.z}.Normalize());
			const float angle        = XMConvertToDegrees(acosf(cos));

			const Vec3 cross          = monsterFront.Cross(Vec3{direction.x, monsterFront.y, direction.z}.Normalize());
			Vec3       beforeRotation = GetOwner()->Transform()->GetRelativeRotation();

			if (fabsf(angle) > 5.f && m_rotationFinish == false)
			{
				beforeRotation.y += XMConvertToRadians(DT * m_rotationSpeed * (cross.y > 0.f ? 1.f : -1.f));
			}
			else if (fabsf(angle) <= 5.f)
			{
				beforeRotation.y += XMConvertToRadians(angle * (cross.y ? 1.f : -1.f));
				m_rotationFinish = true;
			}
			GetOwner()->Transform()->SetRelativeRotation(beforeRotation);
		}
	}
}

void CRun_Gun::Enter()
{
	GetOwner()->Animator3D()->Play(GAME::DEUXIEMIE::GUN::RUN_FORWARD, true);
}

void CRun_Gun::Exit()
{
	GetOwner()->Animator3D()->GetCurAnim()->Reset();
	m_rotationFinish = false;
}
