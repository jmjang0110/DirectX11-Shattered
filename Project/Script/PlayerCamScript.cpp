#include "pch.h"
#include "PlayerCamScript.h"
#include <algorithm>
#include <math.h>

#include <Engine/CKeyMgr.h>
#include <Engine/CCamera.h>
#include <Engine/CMeshRender.h>
#include <Engine/CTransform.h>
#include <Engine/CDevice.h>
#include <Engine/CCore.h>

PlayerCamScript::PlayerCamScript()
	: CScript((int)SCRIPT_TYPE::PLAYERCAMSCRIPT)
	, m_vTargetPos(Vec3(0.f, 0.f, 0.f))
	, m_fDistance(0.f)
	, m_fMinDist(0.f)
	, m_fMaxDist(0.f)
	, m_fCamHeight(180.f)
	, m_eCamMode(CAMERA_MODE::THIRD_PERSON)
	, m_eCamOption(CAMERA_OPTION::DEFAULT)
	, m_bCameraLock_Success(false)
	, m_fCamRotSpeed(5.f)
	, m_pCamLock_Point(nullptr)
{
	SetName(L"PlayerCamScript");

}

PlayerCamScript::PlayerCamScript(const PlayerCamScript& _origin)
	: CScript((int)SCRIPT_TYPE::PLAYERCAMSCRIPT)
	, m_vTargetPos(Vec3(0.f, 0.f, 0.f))
	, m_fDistance(0.f)
	, m_fMinDist(0.f)
	, m_fMaxDist(0.f)
	, m_fCamHeight(180.f)
	, m_eCamMode(CAMERA_MODE::THIRD_PERSON)
	, m_eCamOption(CAMERA_OPTION::DEFAULT)
	, m_bCameraLock_Success(false)
	, m_bRotFinish(false)
	, m_pCamLock_Point(nullptr)

{
	SetName(L"PlayerCamScript");

}

PlayerCamScript::~PlayerCamScript()
{
}


void PlayerCamScript::start()
{
	//if (!m_bFocus_Cursor)
	//{
	//	RECT rect{ 0,0,1600,900 };
	//	ClipCursor(&rect);
	//	SetCursorPos(1600 / 2, 900 / 2);
	//	ShowCursor(SW_HIDE);
	//	m_bFocus_Cursor = true;

	//}


	if (m_pCam == nullptr)
		m_pCam = GetOwner();

	m_vPos = m_vTargetPos;
	m_eCamMode = CAMERA_MODE::THIRD_PERSON; // 3인칭 Default

	SetDistance(500.f); // 캐릭터와 카메라와의 거리 ( 높이는 해당안됨 )
	SetDistanceMinMax(10.f, 600.f);

	if (m_pCamLock_Point == nullptr)
	{
		m_vCamPoint_Color = Vec4(1.f, 1.f, 1.f, 1.f);
		m_pCamLock_Point = new CGameObject;
		m_pCamLock_Point->SetName(L"CameraLock_Point");

		m_pCamLock_Point->AddComponent(new CTransform);
		m_pCamLock_Point->AddComponent(new CMeshRender);

		m_pCamLock_Point->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
		m_pCamLock_Point->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\Std3DWireShader.mtrl"), 0);
		m_pCamLock_Point->MeshRender()->GetDynamicMaterial(0)->SetScalarParam(SCALAR_PARAM::VEC4_0, &m_vCamPoint_Color);

		CSceneMgr::GetInst()->SpawnObject(m_pCamLock_Point, GetOwner()->GetLayerIndex());
	}
}

void PlayerCamScript::update()
{
	
	//if (CCore::GetInst()->GetMainHwnd() != GetFocus())
	//{
	//	ShowCursor(SW_SHOW);
	//	m_bFocus_Cursor = false;

	//}
	//else
	//{
	//	if (!m_bFocus_Cursor)
	//	{
	//		RECT rect{ 0,0,1600,900 };
	//		ClipCursor(&rect);
	//		SetCursorPos(1600 / 2, 900 / 2);
	//		ShowCursor(SW_HIDE);
	//		m_bFocus_Cursor = true;
	//	}
	//}

	if (KEY_TAP(KEY::L))
	{
		/*
			임의로 L 버튼을 누르면 FREE 모드로 전환됩니다.

		*/
		if (m_eCamMode == CAMERA_MODE::THIRD_PERSON)
			m_eCamMode = CAMERA_MODE::FREE;
		else
			m_eCamMode = CAMERA_MODE::THIRD_PERSON;

	}

	switch (m_eCamMode)
	{
	case CAMERA_MODE::FIRST_PERSON: // 1인칭 
	{
		if (m_eCamOption == CAMERA_OPTION::DEFAULT)
			UpdateFirstPersonMode();

	}
	break;
	case CAMERA_MODE::THIRD_PERSON: // 3인칭 
	{
		if (m_eCamOption == CAMERA_OPTION::DEFAULT)
			UpdateThirdPersonMode();
		else if (m_eCamOption == CAMERA_OPTION::CAMERA_LOCK)
		{
			UpdateThirdPersonMode_CameraLock();
			UpdateCameraLockPoint();
		}


	}
	break;
	case CAMERA_MODE::FREE:			// 자유
	{
		if (m_eCamOption == CAMERA_OPTION::DEFAULT)
			UpdateFreeMode();
	}
	break;

	}

}

void PlayerCamScript::lateupdate()
{


	switch (m_eCamMode)
	{
	case CAMERA_MODE::FIRST_PERSON: // 1인칭 
	{
		if (m_eCamOption == CAMERA_OPTION::DEFAULT)
			LateUpdateFirstPersonMode();
	}
	break;
	case CAMERA_MODE::THIRD_PERSON: // 3인칭 
	{
		if (m_eCamOption == CAMERA_OPTION::DEFAULT)
			LeteUpdateThirdPersonMode();
	}
	break;
	case CAMERA_MODE::FREE:			// 자유 
	{

	}
	break;

	}

}

void PlayerCamScript::UpdateFreeMode()
{
	Vec3  vPos = Transform()->GetRelativePos();
	float fSpeed = 750.f;

	if (KEY_PRESSED(KEY::K))
	{
		fSpeed *= 2.f;
	}

	if (PROJ_TYPE::PERSPECTIVE == Camera()->GetProjType())
	{
		if (KEY_PRESSED(KEY::W))
		{
			Vec3 vFront = Transform()->GetWorldFrontDir();
			vPos += DT * vFront * fSpeed;
		}

		if (KEY_PRESSED(KEY::S))
		{
			Vec3 vFront = Transform()->GetWorldFrontDir();
			vPos -= DT * vFront * fSpeed;
		}

		if (KEY_PRESSED(KEY::A))
		{
			Vec3 vRight = Transform()->GetWorldRightDir();
			vPos -= DT * vRight * fSpeed;
		}

		if (KEY_PRESSED(KEY::D))
		{
			Vec3 vRight = Transform()->GetWorldRightDir();
			vPos += DT * vRight * fSpeed;
		}
	}

	else
	{
		if (KEY_PRESSED(KEY::W))
		{
			vPos.y += DT * fSpeed;
		}

		if (KEY_PRESSED(KEY::S))
		{
			vPos.y -= DT * fSpeed;
		}

		if (KEY_PRESSED(KEY::A))
		{
			vPos.x -= DT * fSpeed;
		}

		if (KEY_PRESSED(KEY::D))
		{
			vPos.x += DT * fSpeed;
		}
	}

	if (CKeyMgr::GetInst()->GetKeyState(KEY::RBTN) == KEY_STATE::NONE)
	{
		Vec3 vRot = Transform()->GetRelativeRotation();

		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();
		vRot.y += DT * vMouseDir.x * XM_PI;
		vRot.x -= DT * vMouseDir.y * XM_PI;

		Transform()->SetRelativeRotation(vRot);
	}

	Transform()->SetRelativePos(vPos);
}

void PlayerCamScript::UpdateThirdPersonMode_CameraLock()
{
	Vec3 vPlayerPos = m_vTargetPos;											// 플레이어 위치 
	Vec3 vCamTargetPos = m_pCamLock_TargetObj->Transform()->GetRelativePos();	// 카메라 락 타겟 위치  
	Vec3 vCamPos = UpdateCameraRelativePos();							// 카메라 위치  
	m_pCam->Transform()->SetRelativePos(vCamPos);

	Vec3 directionVec = XMVector3Normalize(GetForwardAxis());
	vCamPos = directionVec * -m_fDistance + vCamPos;
	vCamPos.y += m_fCamHeight;

	Vec3 vTargetDir = vCamTargetPos - vPlayerPos;							// 플레이어 -> 카메라 락 타겟 
	Vec3 vTargetDir_Inv = vTargetDir * -1;										// 플레이어 <- 카메라 락 타겟 
	Vec3 vCamDir = vCamPos - vPlayerPos;									// 플레이어 -> 카메라	
	Vec3 vCamDir_Inv = vCamDir * -1;											// 플레이어 <- 카메라 

	vTargetDir.y = 0.f;
	vTargetDir_Inv.y = 0.f;
	vCamDir.y = 0.f;
	vCamDir_Inv.y = 0.f;

	vTargetDir.Normalize();
	vTargetDir_Inv.Normalize();
	vCamDir.Normalize();
	vCamDir_Inv.Normalize();

	Vec3 vPrev_CamDir = m_vCamDir_CamLock;
	Vec3 vPrev_TargetDir = m_vTargetDir_CamLock;
	m_vCamDir_CamLock = vCamDir;
	m_vTargetDir_CamLock = vTargetDir;

	if (vCamDir != vPrev_CamDir || vTargetDir != vPrev_TargetDir)
		m_bRotFinish = false;

	if (!m_bCameraLock_Success)
	{
		m_bCameraLock_Success = RotateCamera(vCamDir, vTargetDir_Inv);
	}
	else
	{
		if (!m_bRotFinish)
			m_bRotFinish = RotateCamera(vCamDir, vTargetDir_Inv);
	}

	// 위/아래 회전 가능하게 설정  
	Vec3 vRot = GetOwner()->Transform()->GetRelativeRotation();
	Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();
	if (KEY_PRESSED(KEY::RBTN))
	{
		vRot.x -= DT * vMouseDir.y * XM_PI;

		// 0 ~ 360 도 고정 
		if (vRot.x >= XM_2PI)
		{
			float fDiff = vRot.x - XM_2PI;
			vRot.x = fDiff;
		}
		else if (vRot.x <= 0.f)
		{
			float fDiff = XM_2PI - vRot.x;
			vRot.x = fDiff;
		}
		GetOwner()->Transform()->SetRelativeRotation(vRot);		// 카메라 회전 

	}

	// 카메라 위치 (뒤/높이) - 3인칭  
	Translate(GetForwardAxis(), -m_fDistance);							// 카메라가 앞을 바라보는 기준으로 Distance 만큼 [뒤로] 이동
	Vec3 vPos = m_pCam->Transform()->GetRelativePos();
	vPos.y = m_pCam->Transform()->GetRelativePos().y + m_fCamHeight;	// 카메라 높이를 올린다. (상대 - 캐릭터 기준) 
	m_pCam->Transform()->SetRelativePos(vPos);

	// 카메라 잠금 포인트 점 위치 설정  
	if (m_pCamLock_Point)
	{
		float fCamTargetDist = (vCamPos - m_vTargetPos).Length();
		Vec3 vScale = Vec3(30.f, 30.f, 30.f);
		Vec3 vPos = vCamTargetPos;
		Vec3 vDir = directionVec * -1;
		float fDist = 10.f;

		vPos.x += fDist * vDir.x;
		vPos.z += fDist * vDir.y;

		m_pCamLock_Point->Transform()->SetRelativePos(vPos);
		m_pCamLock_Point->Transform()->SetRelativeScale(vScale);

	}
}


bool PlayerCamScript::RotateCamera(Vec3 _vFrom, Vec3 _vTo)
{
	if (_vFrom == _vTo)
		return true;

	Vec3 StartDir = _vFrom;
	StartDir.y = 0.f;
	StartDir.Normalize();

	Vec3 EndDir = _vTo;
	EndDir.y = 0.f;
	EndDir.Normalize();;

	float fCos = StartDir.Dot(EndDir);
	fCos += 0.0001f;
	if (fCos >= 1.f) fCos = 1.f;
	if (fCos <= -1.f) fCos = -1.f;

	float fRad = acosf(fCos);					// 사이각 ( Radian )
	float fAng = fRad * (180.f / XM_PI);		// 사이각 ( Angle )						

	Vec3 vCross = XMVector3Cross(EndDir, StartDir);
	int prevDir = m_iCamRotDir;						// 이전 회전 방향  
	m_iCamRotDir = (vCross.y > 0.f) ? -1 : 1;		// 현재 회전 방향 ( 시계 : 1 / 반시계 : -1 )

	// 회전  
	Vec3 vRot = GetOwner()->Transform()->GetRelativeRotation();

	float fDT = DT * m_fCamRotSpeed * m_iCamRotDir;
	vRot.y += fDT;
	// 0 ~ 360 도 고정 
	if (vRot.y >= XM_2PI)
	{
		float fDiff = vRot.y - XM_2PI;
		vRot.y = fDiff;
	}
	else if (vRot.y <= 0.f)
	{
		float fDiff = XM_2PI + vRot.y;
		vRot.y = fDiff;
	}

	GetOwner()->Transform()->SetRelativeRotation(vRot);

	// 회전 종료 여부 확인 
	Vec3 vForwardAxis = GetForwardAxis();
	vForwardAxis.y = 0.f;
	vForwardAxis.Normalize();
	Vec3 Changed_StartDir = vForwardAxis * -1;
	Vec3 vCross_Check = XMVector3Cross(EndDir, Changed_StartDir);
	int iDir = (vCross_Check.y > 0.f) ? -1 : 1;
	// 회전 종료 - 회전 방향이 바뀜 ( 회전량 초과 ) 
	if (m_iCamRotDir != iDir)
	{
		float fCos = Changed_StartDir.Dot(EndDir);
		if (fCos >= 1.f) fCos = 1.f;
		if (fCos <= -1.f) fCos = -1.f;

		float fRad = acosf(fCos);					// 사이각 ( Radian )
		float fAng = fRad * (180.f / XM_PI);		// 사이각 ( Angle )			

		// 초과 차이 만큼 원상 복구 
		vRot.y += iDir * fRad;
		// 0 ~ 360 도 고정 
		if (vRot.y >= XM_2PI)
		{
			float fDiff = vRot.y - XM_2PI;
			vRot.y = fDiff;
		}
		else if (vRot.y <= 0.f)
		{
			float fDiff = XM_2PI + vRot.y;
			vRot.y = fDiff;
		}

		GetOwner()->Transform()->SetRelativeRotation(vRot);

		return true;
	}

	return false;

}

void PlayerCamScript::UpdateCameraLockPoint()
{
	if (!m_pCamLock_Point)
		return;



}

void PlayerCamScript::UpdateFirstPersonMode()
{

}

void PlayerCamScript::UpdateThirdPersonMode()
{
	// 0. 외부에서 카메라가 찍을 중심 위치(TargetPos) 를 갱신해야한다.
	//m_pCam->SetTarget(Traget Pos);

	// 1. 카메라가 회전한다. 
	Vec3 vRot = m_pCam->Transform()->GetRelativeRotation();
	Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();
	const Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();

	if (KEY_PRESSED(KEY::RBTN))
	{
		//vRot.y += DT * vMouseDir.x * (XM_PI * 0.25f * (1.f / vResolution.y)) * 250.f;
		//vRot.x -= DT * vMouseDir.y * (XM_PI * 0.25f * (1.f / vResolution.x)) * 250.f;

		vRot.y += DT * vMouseDir.x * XM_PI;
		vRot.x -= DT * vMouseDir.y * XM_PI;


		// 0 ~ 360 도 고정 
		if (vRot.y >= XM_2PI)
		{
			float fDiff = vRot.y - XM_2PI;
			vRot.y = fDiff;
		}
		else if (vRot.y <= 0.f)
		{
			float fDiff = XM_2PI - vRot.y;
			vRot.y = fDiff;
		}

		if (vRot.x >= XM_2PI)
		{
			float fDiff = vRot.x - XM_2PI;
			vRot.x = fDiff;
		}
		else if (vRot.x <= 0.f)
		{
			float fDiff = XM_2PI - vRot.x;
			vRot.x = fDiff;
		}

	}


	// 2. 카메라의 위치를 조정한다.
	Vec3 vCamPos = UpdateCameraRelativePos();				// 카메라가 플레이어를 따라가게 한다. 
	m_pCam->Transform()->SetRelativePos(vCamPos);
	//m_pCam->Transform()->SetRelativePos(m_vTargetPos);	// 캐릭터와 카메라의 위치를 일치 


	/*
	   3. 바뀐 위치를 기준으로 카메라 회전각을 조정한다.

			*  회전각을 이때 넣은 이유는 회전각이 달라지면  Forward Axis 도 달라지기 때문에
				카메라를 원래위치로 이동시키는데 문제가 생겨서이다.
	*/
	m_pCam->Transform()->SetRelativeRotation(vRot);		// 카메라 회전 

	// 4. 카메라를 뒤로 이동 ( 3인칭 ) 
	Translate(GetForwardAxis(), -m_fDistance);			// 카메라가 앞을 바라보는 기준으로 Distance 만큼 [뒤로] 이동

	// 5. 카메라 높이 설정 
	Vec3 vPos = m_pCam->Transform()->GetRelativePos();
	vPos.y = m_pCam->Transform()->GetRelativePos().y + m_fCamHeight; // 카메라 높이를 올린다. (상대 - 캐릭터 기준) 
	m_pCam->Transform()->SetRelativePos(vPos);

	// 6. 카메라와 캐릭터와의 거리를 갱신 
	// m_fDistance 를 줄이면 캐릭터와 가까워진다. 
	//pThirdPersonCamera->Approach(-io.MouseWheel * 1.0f);

}

void PlayerCamScript::LateUpdateFirstPersonMode()
{

}

void PlayerCamScript::LeteUpdateThirdPersonMode()
{

}

void PlayerCamScript::SetCameraLockTargetObj(CGameObject* _pObj, Vec3 _vPos)
{
	m_pCamLock_TargetObj = _pObj;
	if (m_pCamLock_TargetObj)
	{
		m_vCamLock_TragetPos = _vPos;

	}
}


void PlayerCamScript::RotateX(float rad)
{
	Vector3 rotation = m_pCam->Transform()->GetRelativeRotation();
	// Limit rotation around the x-axis in radians between [0, pi/3]

	rotation.x += rad;
	if (rotation.x <= 0.f) rotation.x = 0.f;
	else if (rotation.x >= XM_PI / 3.f) rotation.x = XM_PI / 3.f;


	m_pCam->Transform()->SetRelativeRotation(rotation);
	m_pCam->Transform()->SetRelativePos(m_vTargetPos);
	Translate(GetForwardAxis(), -m_fDistance);

}

void PlayerCamScript::RotateY(float rad)
{
	Vector3 rotation = m_pCam->Transform()->GetRelativeRotation();
	rotation.y = XMScalarModAngle(rotation.y + rad);

	m_pCam->Transform()->SetRelativeRotation(rotation);
	m_pCam->Transform()->SetRelativePos(m_vTargetPos);
	Translate(GetForwardAxis(), -m_fDistance);
}

void PlayerCamScript::Approach(float dist)
{
	m_fDistance += dist;
	if (m_fDistance <= 0.f) m_fDistance = 0.f;
	else if (m_fDistance >= m_fMaxDist) m_fDistance = m_fMaxDist;


	m_pCam->Transform()->SetRelativePos(m_vTargetPos);
	Translate(GetForwardAxis(), -m_fDistance);
}

void PlayerCamScript::Translate(const Vec3& direction, float magnitude)
{
	Vec3 directionVec = XMVector3Normalize(direction);

	// SimpleMath 없을 때 방법
	// XMVECTOR newPosition  = XMVectorMultiplyAdd(XMVectorReplicate(magnitude), directionVec,
	// XMLoadFloat3(&m_Position)); XMStoreFloat3(&m_Position, newPosition);
	Vec3 vCurPos = m_pCam->Transform()->GetRelativePos();
	Vec3 vNewPos = directionVec * magnitude + vCurPos;

	m_pCam->Transform()->SetRelativePos(vNewPos);

}

Vec3 PlayerCamScript::GetForwardAxis()
{
	const Vec3 vRotation = m_pCam->Transform()->GetRelativeRotation();

	const XMMATRIX matRotation = DirectX::XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&vRotation));
	Vector3        forward{};
	XMStoreFloat3(&forward, matRotation.r[2]);
	forward.Normalize();

	return forward;
}

Vec3 PlayerCamScript::GetTargetPosition() const
{
	return m_vTargetPos;
}

float PlayerCamScript::GetDistance() const
{
	return m_fDistance;
}

void PlayerCamScript::SetRotationX(float rad)
{
	Vec3 rotation = m_pCam->Transform()->GetRelativeRotation();
	rotation.x += rad;
	if (rotation.x <= 0.f) rotation.x = 0.f;
	else if (rotation.x >= XM_PI / 3.f) rotation.x = XM_PI / 3.f;

	m_pCam->Transform()->SetRelativeRotation(rotation);
	m_pCam->Transform()->SetRelativePos(m_vTargetPos);
	Translate(GetForwardAxis(), -m_fDistance);
}

void PlayerCamScript::SetRotationY(float rad)
{
}

void PlayerCamScript::SetTargetPos(const Vector3& target)
{
	if (m_vTargetPos != target)
		m_vPrevTargetPos = m_vTargetPos;

	m_vTargetPos = target;

}

void PlayerCamScript::SetDistance(float dist)
{
	m_fDistance = dist;

}

void PlayerCamScript::SetDistanceMinMax(float minDist, float maxDist)
{
	m_fMinDist = minDist;
	m_fMaxDist = maxDist;
}

Vec3 PlayerCamScript::UpdateCameraRelativePos()
{
	Vec3 vCamCurPos = GetOwner()->Transform()->GetRelativePos();
	if (vCamCurPos == m_vTargetPos)
		return vCamCurPos;;

	//Translate(GetForwardAxis(), m_fDistance);			// 카메라 원래 위치로 이동 

	// 1. 카메라의 원래 위치로 이동한다. ( 플레이어 위치 기준 m_fDistance 만큼 뒤로, m_fCamHeight 만큼 위로 움직였던걸 원래 위치로.. )
	vCamCurPos += GetForwardAxis() * m_fDistance;
	vCamCurPos.y -= m_fCamHeight;

	if (vCamCurPos == m_vTargetPos)
		return vCamCurPos;;


	// 2. 카메라가 이동할 방향과 거리를 구한다. ( 의도 : 플레이어와의 거리차를 이용해 카메라가 플레이어를 따라가게 만든다. )
	Vec3 vDist = m_vTargetPos - vCamCurPos;
	Vec3 vLength = vDist;
	vLength = Vec3(abs(vLength.x), abs(vLength.y), abs(vLength.z)); // 이동 거리 길이
	Vec3 vVector = vDist.Normalize();									// 이동 방향 

	// 카메라가 플레이어 위치를 따라간다. 
	// 흠.... Length 가 짧아질수록 시간단축이 굉장히 짧아지니까... 가까워 질수록 너무 느리게 다가간다.
	// 일정 거리 차이가 있으면 캐릭터한테 가는 속도를 고정하자..
	// 멀면 빨리, 가까우면 일정속도로 플레어한테 카메라가 움직여야지.
	// 어떻게 할까././

	// 3. 거리 차만큼 카메라 위치를 조정한다. ( 단, 거리차가 너무 작으면 다가가는 속도도 그만큼 작아지므로 지정해준다. )
	float fXSpeed = 0.1f;
	if (vDist.x <= 10.f)
		fXSpeed = 3.f;

	float fYSpeed = 0.01f;
	if (vDist.y <= 10.f)
		fYSpeed = 3.f;

	float fZSpeed = 0.01f;
	if (vDist.z <= 10.f)
		fZSpeed = 3.f;

	vCamCurPos.x += DT * vLength.x * vVector.x * fXSpeed;
	vCamCurPos.y += DT * vLength.y * vVector.y * fYSpeed;
	vCamCurPos.z += DT * vLength.z * vVector.z * fZSpeed;

	if (vVector.x < 0.f)
	{
		if (vCamCurPos.x <= m_vTargetPos.x)
			vCamCurPos.x = m_vTargetPos.x;
	}
	else
	{
		if (vCamCurPos.x >= m_vTargetPos.x)
			vCamCurPos.x = m_vTargetPos.x;
	}

	if (vVector.y < 0.f)
	{
		if (vCamCurPos.y <= m_vTargetPos.y)
			vCamCurPos.y = m_vTargetPos.y;
	}
	else
	{
		if (vCamCurPos.y >= m_vTargetPos.y)
			vCamCurPos.y = m_vTargetPos.y;
	}

	if (vVector.z < 0.f)
	{
		if (vCamCurPos.z <= m_vTargetPos.z)
			vCamCurPos.z = m_vTargetPos.z;
	}
	else
	{
		if (vCamCurPos.z >= m_vTargetPos.z)
			vCamCurPos.z = m_vTargetPos.z;
	}


	return vCamCurPos;

}

void PlayerCamScript::Reset_CameraLock()
{
	m_bCameraLock_Success = false;
	if (m_pCamLock_Point)
	{
		m_pCamLock_Point->Activate();

	}
}
void PlayerCamScript::Reset_Deafult()
{
	if (m_pCamLock_Point)
	{
		m_pCamLock_Point->Deactivate();

	}
}

void PlayerCamScript::OnCollisionEnter(CGameObject* _OtherObject)
{
}

void PlayerCamScript::OnCollision(CGameObject* _OtherObject)
{
}

void PlayerCamScript::OnCollisionExit(CGameObject* _OtherObject)
{
}


void PlayerCamScript::SaveToScene(FILE* _pFile)
{
}

void PlayerCamScript::LoadFromScene(FILE* _pFile)
{
}