#include "pch.h"
#include "CEditorCamScript.h"

#include <Engine/CKeyMgr.h>
#include <Engine/CTimeMgr.h>

#include <Engine/CTransform.h>
#include <Engine/CCamera.h>

CEditorCamScript::CEditorCamScript()
	: CScript(-1)
	, m_fSpeed(200.f)
	, m_projectionType{PROJ_TYPE::PERSPECTIVE} {}

CEditorCamScript::~CEditorCamScript() {}

void CEditorCamScript::update()
{
	if (KEY_PRESSED(KEY::LCTRL) && KEY_TAP(KEY::F10))
	{
		m_projectionType = Camera()->GetProjType() == PROJ_TYPE::PERSPECTIVE
			                   ? PROJ_TYPE::ORTHOGRAPHIC
			                   : PROJ_TYPE::PERSPECTIVE;
		LOG_INFO("CAMERA PROJECTION : %s", (m_projectionType ==PROJ_TYPE::PERSPECTIVE)? "PERSPECTIVE":"ORTHOGRAPHIC");
		Camera()->SetProjType(m_projectionType);
	}
	if (KEY_PRESSED(KEY::LCTRL) && KEY_TAP(KEY::F11))
	{
		LOG_INFO("CAMERA POS/ROTATION RESET");
		Transform()->SetRelativePos({0.f, 0.f, 0.f});
		Transform()->SetRelativeRotation({0.f, 0.f, 0.f});
	}

	if (KEY_PRESSED(KEY::LCTRL) && KEY_TAP(KEY::F12))
	{
		m_bEnableInput = !m_bEnableInput;
		LOG_INFO(m_bEnableInput ? "EDITOR CAMERA MOVE ENABLED" : "EDITOR CAMERA MOVE DISABLED");
	}

	if (false == m_bEnableInput)
	{
		return;
	}

	Vec3  vPos   = Transform()->GetRelativePos();
	float fSpeed = m_fSpeed;

	if (KEY_PRESSED(KEY::LSHFT))
	{
		fSpeed *= 5.f;
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

		if (KEY_PRESSED(KEY::Q))
		{
			Vec3 vUp = Transform()->GetWorldUpDir();
			vPos -= DT * vUp * fSpeed;
		}
		if (KEY_PRESSED(KEY::E))
		{
			Vec3 vUp = Transform()->GetWorldUpDir();
			vPos += DT * vUp * fSpeed;
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


	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec3 vRot = Transform()->GetRelativeRotation();

		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();
		vRot.y += DT * vMouseDir.x * XM_PI;
		vRot.x -= DT * vMouseDir.y * XM_PI;

		Transform()->SetRelativeRotation(vRot);
	}

	Transform()->SetRelativePos(vPos);
}
