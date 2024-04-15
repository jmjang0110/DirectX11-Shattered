#include "pch.h"
#include "SocketColliderScript.h"

#include <Engine/CTransform.h>
#include <Engine/CAnimator3D.h>
#include <Engine/CCollider3D.h>



SocketColliderScript::SocketColliderScript()
	: CScript((int)SCRIPT_TYPE::SOCKETCOLLIDERSCRIPT)
	, m_vSocketPos(Vec3(0.f, 0.f, 0.f))
	, m_vSocketRot(Vec3(0.f, 0.f, 0.f))
	, m_vSocket_OffsetPos(Vec3(0.f, 0.f, 0.f))
	, m_SocketMatrix(Matrix{})
	, m_iSocketIdx(0)
	, m_pColliderObj(nullptr)

{
	SetName(L"SocketColliderScript");
}

SocketColliderScript::SocketColliderScript(const SocketColliderScript& _origin)
	: CScript((int)SCRIPT_TYPE::SOCKETCOLLIDERSCRIPT)

{
	SetName(L"SocketColliderScript");
}


SocketColliderScript::~SocketColliderScript()
{
}


void SocketColliderScript::start()
{
	if (!m_pColliderObj)
	{
		m_pColliderObj = GetOwner();
		m_pColliderObj->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 50.f));
	}

	if (!m_pAnimObj)
		m_pAnimObj = GetOwner()->GetParent();


	if (!m_pColliderObj->Collider3D())
	{
		m_pColliderObj->AddComponent(new CCollider3D);
		m_pColliderObj->Collider3D()->SetOffsetScale(Vec3(100.f, 100.f, 100.f));
	}
	if (!m_pColliderObj->MeshRender())
	{
		m_pColliderObj->AddComponent(new CMeshRender);
		m_pColliderObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
		m_pColliderObj->MeshRender()->
			SetSharedMaterial(CResMgr::GetInst()->FindRes<
				CMaterial>(L"material\\Std3DMtrl.mtrl"),
				0);
	}


}

void SocketColliderScript::update()
{
	if (!m_pColliderObj || !m_pAnimObj)
	{
		if (!m_pColliderObj)
			m_pColliderObj = GetOwner();

		if (!m_pAnimObj)
		{
			if (GetOwner()->GetParent())
			{
				if (GetOwner()->GetParent()->Animator3D())
					m_pAnimObj = GetOwner()->GetParent();
			}
			else
				return;
		}
		return;
	}

	CAnimator3D* pAnimator3D = m_pAnimObj->Animator3D();
	if (pAnimator3D)
	{
		CAnimation3D* pAnim = pAnimator3D->GetCurAnim();
		if (pAnim)
		{
			m_SocketMatrix = pAnimator3D->GetSocket(m_iSocketIdx);
			m_vSocketRot = DecomposeRotMat(m_SocketMatrix);
			m_vSocketPos = Vec3(m_SocketMatrix._41, m_SocketMatrix._42, m_SocketMatrix._43);


			m_pColliderObj->Transform()->SetRelativeRotation(m_vSocketRot);	// 회전 적용 
			m_pColliderObj->Transform()->SetRelativePos(m_vSocketPos);		// 메인 위치 적용 

			m_pColliderObj->Collider3D()->SetOffsetPos(m_vSocket_OffsetPos); // 메인 위치 + OffsetPos  

		}
	}
}

int SocketColliderScript::GetvecSocketMaxSize()
{
	int size = 0;

	if (m_pAnimObj)
		size = m_pAnimObj->Animator3D()->GetvecSocketMatSize();

	return size;

}

void SocketColliderScript::lateupdate()
{
}

void SocketColliderScript::OnCollisionEnter(CGameObject* _OtherObject)
{
}

void SocketColliderScript::OnCollision(CGameObject* _OtherObject)
{
}

void SocketColliderScript::OnCollisionExit(CGameObject* _OtherObject)
{
}

void SocketColliderScript::SaveToScene(FILE* _pFile)
{
}

void SocketColliderScript::LoadFromScene(FILE* _pFile)
{
}


