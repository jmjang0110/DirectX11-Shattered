#include "pch.h"
#include "SwordTrailScript.h"

#include <Engine/CMeshRender.h>
#include <Engine/CMesh.h>


SwordTrailScript::SwordTrailScript()
	: CScript((int)SCRIPT_TYPE::SWORDTRAILSCRIPT)
	, m_bStart(false)
	, m_vForward(Vec3(0.f, 0.f, 0.f))
	, m_vBackward(Vec3(0.f, 0.f, 0.f))
	, m_vStartPos(Vec3(0.f, 0.f, 0.f))
	, m_vEndPos(Vec3(0.f, 0.f, 0.f))
	, m_vCurPos_Local(Vec3(0.f, 0.f, 0.f))
	, m_vPrevPos_Local(Vec3(0.f, 0.f, 0.f))
	, m_vCurPos_World(Vec3(0.f, 0.f, 0.f))
	, m_vPrevPos_World(Vec3(0.f, 0.f, 0.f))

	, m_vSword_Pos(Vec3(0.f, 0.f, 0.f))
	, m_vSword_Rot(Vec3(0.f, 0.f, 0.f))

{
}


SwordTrailScript::SwordTrailScript(const SwordTrailScript* _origin)
	: CScript((int)SCRIPT_TYPE::SWORDTRAILSCRIPT)
	, m_bStart(false)
	, m_vForward(Vec3(0.f, 0.f, 0.f))
	, m_vBackward(Vec3(0.f, 0.f, 0.f))
	, m_vStartPos(Vec3(0.f, 0.f, 0.f))
	, m_vEndPos(Vec3(0.f, 0.f, 0.f))
	, m_vCurPos_Local(Vec3(0.f, 0.f, 0.f))
	, m_vPrevPos_Local(Vec3(0.f, 0.f, 0.f))
	, m_vCurPos_World(Vec3(0.f, 0.f, 0.f))
	, m_vPrevPos_World(Vec3(0.f, 0.f, 0.f))
	, m_vSword_Pos(Vec3(0.f, 0.f, 0.f))
	, m_vSword_Rot(Vec3(0.f, 0.f, 0.f))

{
}


SwordTrailScript::~SwordTrailScript()
{
}



void SwordTrailScript::start()
{
	m_pTarget = GetOwner();
	GetOwner()->AddComponent(new CMeshRender);

	Ptr<CTexture> pTex = CResMgr::GetInst()->Load<CTexture>(L"texture\\Sword_Trail_4.png",
		L"texture\\Sword_Trail_4.png");
	Ptr<CTexture> pPostProcess = CResMgr::GetInst()->FindRes<CTexture>(L"PostProcessTex");

	GetOwner()->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh_Trail"));
	//GetOwner()->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\Std3DWireShader.mtrl"), 0);
	GetOwner()->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\SwordTrailMtrl.mtrl"), 0);
	GetOwner()->MeshRender()->GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, pTex);
	GetOwner()->MeshRender()->GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_1, pPostProcess);


	//GetOwner()->MeshRender()->GetDynamicMaterial(0)->SetScalarParam(SCALAR_PARAM::, pMagicCircle);


		//L"material\\Std3DMtrl.mtrl"
	int iVtxCnt = m_pTarget->MeshRender()->GetMesh()->GetVtxCount();
	m_iUpdateVtx_Max = (iVtxCnt - 2) / 2;
}

void SwordTrailScript::update()
{


	// 정점 정보 업데이트 
	UpdateVertexInfo_Start();		// 처음 시작에서 끝까지 늘어남
	UpdateVertexInfo_End();			// 처음 시작에서 끝까지 줄어듬 

	m_pTarget->MeshRender()->GetMesh()->UpdateVertexBuffer_Dynamic(); // CPU -> GPU ( 정점 정보 )
	m_pTarget->Transform()->SetRelativePos(m_vStartPos);


}

void SwordTrailScript::lateupdate()
{
}

void SwordTrailScript::OnCollisionEnter(CGameObject* _OtherObject)
{
}

void SwordTrailScript::OnCollision(CGameObject* _OtherObject)
{
}

void SwordTrailScript::OnCollisionExit(CGameObject* _OtherObject)
{
}

void SwordTrailScript::SaveToScene(FILE* _pFile)
{
}

void SwordTrailScript::LoadFromScene(FILE* _pFile)
{
}


void SwordTrailScript::UpdateVertexInfo_Start()
{
	// 시작에서 끝 까지 늘어남
	if (!m_bStart || m_bFinish_Start)
		return;

	int iVtxCnt = m_pTarget->MeshRender()->GetMesh()->GetVtxCount();
	Vtx* pVtxMem = m_pTarget->MeshRender()->GetMesh()->GetVtxSysMem();

	//Vec3 vCurPos		= m_pTarget->Transform()->GetRelativePos();
	Vec3 vCurPos = m_vSword_Pos;

	m_vPrevPos_World = m_vCurPos_World;
	m_vCurPos_World = vCurPos;

	if (!m_bStart_Setting)
	{
		m_pTarget->MeshRender()->Activate();

		// 시작 세팅  
		m_bStart_Setting = true;

		m_pTarget->Transform()->SetRelativePos(vCurPos);
		m_vStartPos = vCurPos;
		m_iUpdateVtx_Cnt = 0;

		m_vPrevPos_World = vCurPos;
		m_vCurPos_World = vCurPos;

		m_vCurPos_Local = Vec3(0.f, 0.f, 0.f);

	}

	// 궤적 방향 
	Vec3    vTrajectory_Dist = m_vCurPos_World - m_vPrevPos_World;
	Vec3	vTrajectory = (m_vCurPos_World - m_vPrevPos_World).Normalize();
	float	vTrajectpry_Length = vTrajectory_Dist.Length();

	// World -> Local 길이 계산
	Vec3	vSword_Scale = m_pTarget->Transform()->GetRelativeScale();
	Vec3	vDistRaitio_Local = vTrajectory_Dist / vSword_Scale;
	float	vDistLength_Local = vDistRaitio_Local.Length();


	// 궤적 기준 앞/뒤 방향 
	m_vForward = m_vSword_Forward;
	m_vBackward = m_vForward * -1;

	int iVtxIdx_L = (m_iUpdateVtx_Cnt * 2);
	int iVtxIdx_R = (m_iUpdateVtx_Cnt * 2) + 1;

	pVtxMem[iVtxIdx_L].vPos = m_vCurPos_Local + 0.5f * m_vForward;
	pVtxMem[iVtxIdx_R].vPos = m_vCurPos_Local + 0.5f * m_vBackward;
	m_vCurPos_Local += vDistLength_Local * vTrajectory;

	if (m_iUpdateVtx_Cnt < m_iUpdateVtx_Max)
		m_iUpdateVtx_Cnt++;
	else
	{
		//m_bStart = false;
		m_bStart_Setting = false;
		m_bFinish_Start = true;
		m_iShrink_Idx = 0;

	}

	// 나머지 정점을 마지막에 업데이트한 정점과 일치시킨다. 
	for (int i = iVtxIdx_R + 1; i <= iVtxCnt - 2; i += 2)
	{
		pVtxMem[i].vPos = pVtxMem[iVtxIdx_L].vPos;
		pVtxMem[i + 1].vPos = pVtxMem[iVtxIdx_R].vPos;
	}


	return;

}

void SwordTrailScript::UpdateVertexInfo_End()
{
	// 시작에서 끝까지 줄어듬 
	if (!m_bFinish_Start)
		return;


	int iVtxCnt = m_pTarget->MeshRender()->GetMesh()->GetVtxCount();
	Vtx* pVtxMem = m_pTarget->MeshRender()->GetMesh()->GetVtxSysMem();

	// 기존 정점을 처음에서 끝으로 줄어들게 만든다. 
	if (m_iShrink_Idx >= 2)
	{
		pVtxMem[m_iShrink_Idx - 2].vPos = pVtxMem[m_iShrink_Idx + 2].vPos;
		pVtxMem[m_iShrink_Idx - 1].vPos = pVtxMem[m_iShrink_Idx + 3].vPos;


		for (int i = 0; i < m_iShrink_Idx; i += 2)
		{
			pVtxMem[i].vPos = pVtxMem[m_iShrink_Idx + 2].vPos;
			pVtxMem[i + 1].vPos = pVtxMem[m_iShrink_Idx + 3].vPos;
		}

	}


	m_iShrink_Idx += 2;
	if (m_iShrink_Idx >= iVtxCnt - 2)
	{
		m_pTarget->MeshRender()->Deactivate();

		m_bFinish_Start = false;
		m_bStart = false;

	}


	return;
}

void SwordTrailScript::Reset()
{
	m_pTarget->MeshRender()->Activate();
	m_bFinish_Start = false;
	m_bStart = false;

	// 시작 세팅  
	m_bStart_Setting = false;
	m_iUpdateVtx_Cnt = 0;

	m_vCurPos_Local = Vec3(0.f, 0.f, 0.f);
}

