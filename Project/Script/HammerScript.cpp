#include "pch.h"
#include "HammerScript.h"

#include <Engine/CAnimator3D.h>
#include <Engine/CCollider3D.h>

#include "PlayerScript.h"

HammerScript::HammerScript()
	: CScript((int)SCRIPT_TYPE::HAMMERSCRIPT)
{
}

HammerScript::HammerScript(const HammerScript& _origin)
	: CScript((int)SCRIPT_TYPE::HAMMERSCRIPT)
{
}

HammerScript::~HammerScript()
{
}

void HammerScript::start()
{
}

void HammerScript::update()
{
	CGameObject* pHammer     = GetOwner()->GetParent();
	CAnimator3D* pAnimator3D = pHammer->GetParent()->Animator3D();
	if (pAnimator3D && Collider3D())
	{
		CAnimation3D* pAnim = pAnimator3D->GetCurAnim();
		if (pAnim)
		{
			if (pAnimator3D->GetvecSocketMatSize() <= 0)
				return;

			int    SocketIdx   = 59;
			Matrix SocketMat   = pAnimator3D->GetSocket(SocketIdx);
			Vec3   SocketRot   = DecomposeRotMat(SocketMat);
			Vec3   SocketTrans = Vec3(SocketMat._41, SocketMat._42, SocketMat._43);

			// 회전 적용 
			Transform()->SetRelativeRotation(SocketRot);

			Vec3 vScale       = Collider3D()->GetOffsetScale();
			Vec3 vForward     = Transform()->GetWorldFrontDir();
			Vec3 vOffsetTrans = vForward * (vScale.z / 2.f);

			// 위치 적용 
			Transform()->SetRelativePos(SocketTrans + m_vOffsetPos); // 메인 위치
			Collider3D()->SetOffsetPos(vOffsetTrans); // 임의로 위치 조정  
		}
	}
}

void HammerScript::lateupdate()
{
}

void HammerScript::OnCollisionEnter(CGameObject* _OtherObject)
{
}

void HammerScript::OnCollision(CGameObject* _OtherObject)
{
	CScript::OnCollision(_OtherObject);
}

void HammerScript::OnCollisionExit(CGameObject* _OtherObject)
{
	CScript::OnCollisionExit(_OtherObject);
}
