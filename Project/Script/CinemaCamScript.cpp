#include "pch.h"
#include "CinemaCamScript.h"
#include "CObjectManager.h"

CinemaCamScript::CinemaCamScript()
	: CScript((int)SCRIPT_TYPE::CINEMACAMSCRIPT)
{
	SetName(L"CinemaCamScript");
}

CinemaCamScript::CinemaCamScript(const CinemaCamScript& _origin)
	: CScript((int)SCRIPT_TYPE::CINEMACAMSCRIPT)

{
	SetName(L"CinemaCamScript");
}


CinemaCamScript::~CinemaCamScript()
{
}

void CinemaCamScript::start()
{
	if (!m_pTargetObject)
	{
		CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(GAME::LAYER::PLAYER);
		if (pLayer)
			m_pTargetObject = pLayer->FindRootObject(L"player");

	}

	


	//SetLoopRepeat(true);
}

void CinemaCamScript::update()
{
	CObjectManager::AddScriptEvent(this, &CinemaCamScript::UpdatePosition);
	CObjectManager::AddScriptEvent(this, &CinemaCamScript::UpdateRotation);
	//UpdatePosition();
	//UpdateRotation();
}


void CinemaCamScript::lateupdate()
{
}


void CinemaCamScript::UpdatePosition()
{
	if (m_vecCatmullRom.empty())
		return;

	// CatmullRom 에 저장된 위치를 이용해 카메라를 움직인다. 
	for (int i = 0; i < m_vecCatmullRom.size(); ++i)
	{
		if (!m_vecCatmullRom[i].m_bStart)
			continue;
		// CatmullRom[i] 번째 움직임이 끝났다면 다음 움직임으로 전환  
		if (m_vecCatmullRom[i].m_bIsEnd)
		{
			if (i + 1 <= m_vecCatmullRom.size() - 1)
			{
				m_vecCatmullRom[i + 1].m_bStart = true;
				continue;
			}
		}

		m_vecCatmullRom[i].Update();
		Vec3 vPos = m_vecCatmullRom[i].CalculatePosByCatmullRom();
		GetOwner()->Transform()->SetRelativePos(vPos);
	}


	// [0] - > [1] - > [2] ---...[0] - > ...
	if (m_bLoopRepeat
		&& m_vecCatmullRom[m_vecCatmullRom.size() - 1].m_bIsEnd)
	{
		ResetCatmullRom();
	}
	else if (m_vecCatmullRom[m_vecCatmullRom.size() - 1].m_bIsEnd)
		m_bFinish = true;

}

void CinemaCamScript::UpdateRotation()
{
	if (!m_pTargetObject)
	{
		CLayer* pLayer  = CSceneMgr::GetInst()->GetCurScene()->GetLayer(L"Player");
		m_pTargetObject = pLayer->FindRootObject(L"player");
		return;
	}

	Vec3 vTargetPos = m_pTargetObject->Transform()->GetWorldPos();
	Vec3 vCamPos    = GetOwner()->Transform()->GetWorldPos();
	Vec3 vTargetDir = vTargetPos - vCamPos;

	Vec3 vFrom = Vec3(0.f, 0.f, 1.f);
	Vec3 vTo   = vTargetDir.Normalize();
	RotateCamera(vFrom, vTo);
}

bool CinemaCamScript::RotateCamera(Vec3 _vFrom, Vec3 _vTo)
{
	if (_vFrom == _vTo)
		return true;

	int iCamRotationDir = 1;

#pragma region Y_Axis_Rotate
	// Y 축 회전 업데이트  
	Vec3  vFrom_Y = Vec3(_vFrom.x, 0.f, _vFrom.z).Normalize();
	Vec3  vTo_Y   = Vec3(_vTo.x, 0.f, _vTo.z).Normalize();
	float fRad_Y  = GetAngleRadian(vFrom_Y, vTo_Y);

	Vec3 vCross_Y   = XMVector3Normalize(XMVector3Cross(vTo_Y, vFrom_Y));
	iCamRotationDir = (vCross_Y.y >= 0.f) ? -1 : 1;		// 현재 회전 방향 ( 시계 : 1 / 반시계 : -1 )

	// 회전  
	Vec3 vRot = GetOwner()->Transform()->GetRelativeRotation();
	{
		vRot.y = fRad_Y * iCamRotationDir;
	}
#pragma endregion

#pragma region X_Axis_Rotate
	// X 축 회전 업데이트 
	/*
			--------> ( vFrom_X )
			↘
			  ↘
				↘
				  ↘ ( vTo_X )
	*/
	Vec3  vFrom_X = Vec3(_vTo.x, 0.f, _vTo.z);
	Vec3  vTo_X   = Vec3(_vTo.x, _vTo.y, _vTo.z);
	float fRad_X  = GetAngleRadian(vFrom_X, vTo_X);

	Vec3 vCross_X   = XMVector3Cross(vTo_X, vFrom_X);
	iCamRotationDir = (vCross_X.y > 0.f) ? -1 : 1;		// 현재 회전 방향 ( 시계 : 1 / 반시계 : -1 )
	if (_vTo.y > 0.f)
		iCamRotationDir *= -1;

	// 회전  
	{
		vRot.x = fRad_X * iCamRotationDir;
	}
#pragma endregion

	GetOwner()->Transform()->SetRelativeRotation(vRot);


	return true;
}

float CinemaCamScript::GetAngleRadian(Vec3 _v1, Vec3 _v2)
{
	// Degree : -180 ~ 180
	Vec3 vFrom = _v1;
	vFrom.Normalize();

	Vec3 vTo = _v2;
	vTo.Normalize();

	float fCos = vFrom.Dot(vTo);
	if (fCos >= 1.f) fCos = 1.f;
	if (fCos <= -1.f) fCos = -1.f;

	float fRad = acosf(fCos);					// 사이각 ( Radian )
	float fAng = fRad * (180.f / XM_PI);		// 사이각 ( Angle )	}

	if (fAng == -180.f)
		fRad = XM_PI;


	return fRad;
}

void CinemaCamScript::CreateCatmullRomRoute(Vec3 _vPos0, vector<Vec3> _vecMainPos, Vec3 _vPos1)
{
	if (_vecMainPos.empty() || _vecMainPos.size() == 1)
		return;

	// [0] ---- Mainpos[0] -> MainPos[1] ... -> [1]
	vector<Vec3> MainPos = _vecMainPos;
	Vec3         vPos0   = _vPos0;
	Vec3         vPos1   = _vPos1;

	int EndIdx = MainPos.size() - 1;

	for (int i = 0; i < EndIdx; ++i)
	{
		CatmullRomSpline CatmullRom;
		CatmullRom.m_bIsEnd = false;
		CatmullRom.m_bStart = false;

		Vec3 from = Vec3{};
		Vec3 to   = Vec3{};
		if (i + 1 <= EndIdx)
		{
			from = MainPos[i];
			to   = MainPos[i + 1];
		}

		// First 
		if (i == 0)
		{
			if (i + 2 > EndIdx)
				CatmullRom.CreateRoute(vPos0, from, to, vPos1);
			else
				CatmullRom.CreateRoute(vPos0, from, to, MainPos[i + 2]);
		}
		else
		{
			if (i + 2 <= EndIdx)
				CatmullRom.CreateRoute(MainPos[i - 1], from, to, MainPos[i + 2]);
			else
				CatmullRom.CreateRoute(MainPos[i - 1], from, to, vPos1);
		}

		AddCatmullRomSpline(CatmullRom);
	}


	return;
}

Vec3 CinemaCamScript::CalCulateCatmullRom(Vec3 _p0, Vec3 _p1, Vec3 _p2, Vec3 _p3, float _t)
{
	Vec3 Result;

	float t2 = _t * _t;
	float t3 = t2 * _t;

	float P0 = -t3 + 2.0f * t2 - _t;
	float P1 = 3.0f * t3 - 5.0f * t2 + 2.0f;
	float P2 = -3.0f * t3 + 4.0f * t2 + _t;
	float P3 = t3 - t2;

	Result.x = (P0 * _p0.x + P1 * _p1.x + P2 * _p2.x + P3 * _p3.x) * 0.5f;
	Result.y = (P0 * _p0.y + P1 * _p1.y + P2 * _p2.y + P3 * _p3.y) * 0.5f;
	Result.z = (P0 * _p0.z + P1 * _p1.z + P2 * _p2.z + P3 * _p3.z) * 0.5f;
	//Result.w = (P0 * _p0.w + P1 * _p1.w + P2 * _p2.w + P3 * _p3.w) * 0.5f;

	return Result;
}

void CinemaCamScript::ClearCatmullRom()
{
	vector<CatmullRomSpline> empty;
	m_vecCatmullRom.clear();
	m_vecCatmullRom = empty;
}


void CinemaCamScript::ResetCatmullRom()
{
	for (int i = 0; i < m_vecCatmullRom.size(); ++i)
	{
		m_vecCatmullRom[i].m_bStart                             = false;
		m_vecCatmullRom[i].m_bIsEnd                             = false;
		m_vecCatmullRom[i].m_fInterPolation_Control_Coefficient = 0.f;
	}

	if (!m_vecCatmullRom.empty())
		m_vecCatmullRom[0].m_bStart = true;
}

void CinemaCamScript::CreateStartMap02Route(Vec3 _vStartPos)
{
	if (m_pTargetObject)
	{
		Vec3 vPos;
		Vec3 vForwardAxis = m_pTargetObject->Transform()->GetWorldFrontDir();
		Vec3 vBackWardAxis = vForwardAxis * -1;
		Vec3 vRightAxis = m_pTargetObject->Transform()->GetWorldRightDir();
		Vec3 vLeftAxis = vRightAxis * -1;

		//Vec3 vPlayerPos = m_pTargetObject->Transform()->GetRelativePos();
		Vec3 vPos0 = _vStartPos;

		vector<Vec3> MainPos;
		vPos = vPos0 + vBackWardAxis * 2000.f;
		MainPos.push_back(vPos);
		vPos = vPos + vBackWardAxis * 1000.f;
		vPos = vPos + vLeftAxis * 1000.f;
		vPos.y += 200.f;
		MainPos.push_back(vPos);
		vPos = vPos + vRightAxis * 2000.f + vBackWardAxis * 2000.f;
		vPos.y += 400.f;
		MainPos.push_back(vPos);
		vPos = vPos + vBackWardAxis * 300.f + vLeftAxis * 1000.f;
		vPos.y += 800.f;
		MainPos.push_back(vPos);
		vPos = vPos + vBackWardAxis * 3000.f;
		vPos.y += 800.f;
		MainPos.push_back(vPos);
		vPos = vPos + vBackWardAxis * 1500.f;
		vPos.y -= 500.f;
		MainPos.push_back(vPos);
		vPos = vPos + vBackWardAxis * 1000.f;
		vPos.y -= 600.f;
		MainPos.push_back(vPos);



		Vec3 vPos1 = vPos + vBackWardAxis * 800.f;
		CreateCatmullRomRoute(vPos0, MainPos, vPos1);
		m_vecCatmullRom[0].m_bStart = true;
		m_vecCatmullRom[0].SetSpeed(0.6f);
		m_vecCatmullRom[1].SetSpeed(0.7f);
		m_vecCatmullRom[2].SetSpeed(0.7f);
		m_vecCatmullRom[3].SetSpeed(0.6f);
		m_vecCatmullRom[4].SetSpeed(0.5f);
		m_vecCatmullRom[5].SetSpeed(0.2f);

	}
	else
	{
		Vec3 vPos0 = Vec3(0.f, 0.f, 0.f);
		Vec3 vPos1 = Vec3(1000.f, 1000.f, 1000.f);

		vector<Vec3> mainPos;
		mainPos.push_back(Vec3(500.f, 500.f, 500.f));
		mainPos.push_back(Vec3(100.f, 100.f, 100.f));
		mainPos.push_back(Vec3(-500.f, -500.f, 100.f));

		CreateCatmullRomRoute(vPos0, mainPos, vPos1);
		m_vecCatmullRom[0].m_bStart = true;
	}
}

void CinemaCamScript::OnCollisionEnter(CGameObject* _OtherObject)
{
}

void CinemaCamScript::OnCollision(CGameObject* _OtherObject)
{
}

void CinemaCamScript::OnCollisionExit(CGameObject* _OtherObject)
{
}

void CinemaCamScript::SaveToScene(FILE* _pFile)
{
}

void CinemaCamScript::LoadFromScene(FILE* _pFile)
{
}
