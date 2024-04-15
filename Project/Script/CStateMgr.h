#pragma once
#include <Engine/CEntity.h>
#include "CObjKeyMgr.h"

class CStateMgr
	: public CEntity
{
private:
	CGameObject* m_pTrailSocketCollider;

	CGameObject*			m_pOwnerObj;
	CGameObject*			m_pCamera;


	wstring					m_sPrevState;
	wstring					m_sCurstate;

	tKey_Zip				m_tCurKeyInfo;
	tKey_Zip				m_tPrevKeyInfo;


	bool					m_bOnGround;	// 땅위에 있는지 확인 

	// 카메라 잠금 모드 설정을 위한 변수 
	bool					m_bCameraLock;			// 카메라 잠금 설정 모드인지 확인 
	Vec3					m_vCamLock_TargetPos;	// 카메라 잠금 모드 설정 기준 좌표 
	CGameObject*			m_pCamLock_TargetObj;	// 카메라 잠금 모드로 설정된 기준 오브젝트 저장 



public:
	void	Update();
	void	LateUpdate();


public:
	void InitState(CGameObject* _pOwner);
	void ChangeCurStateType(wstring _StateTypeName); 	// 외부에서 변경되었을 시 Curstate를 변경하기 위해서 생성된 함수

	


public:
	// [ SET PART ]
	void SetCurKeyState(KEY_STATE _eKeyState)		{ m_tCurKeyInfo.eKeyState = _eKeyState; }
	void SetCurKey(KEY _eKey)						{ m_tCurKeyInfo.eKey = _eKey; }
	void SetKeyInfo(KEY_STATE _eKeyState, KEY _eKey, tKeyOptionFlags_Zip _tFlags);
	void SetCurKeyInfo(tKey_Zip _tCurKeyInfo)		{ m_tCurKeyInfo = _tCurKeyInfo; }
	void SetPrevKeyInfo(tKey_Zip _tPrevKeyInfo)		{ m_tPrevKeyInfo = _tPrevKeyInfo; }
	void SetCameraLock(bool _b);
	void SetCameraLockTargetPos(Vec3 _vPos)			{ m_vCamLock_TargetPos = _vPos; }
	void SetCamera(CGameObject* _pCam)				{ m_pCamera = _pCam; }

	// [ GET PART ]
	wstring		 GetCurstate()							{ return m_sCurstate; }
	wstring		 GetPrevState()							{ return m_sPrevState; }
	tKey_Zip	 GetCurKeyInfo()						{ return m_tCurKeyInfo; }
	tKey_Zip	 GetPrevKeyInfo()						{ return m_tPrevKeyInfo; }
	bool		 IsCameraLock()							{ return m_bCameraLock; }
	Vec3		 GetCurCameraLockTargetPos()			{ return m_vCamLock_TargetPos; }
	CGameObject* GetCamera()							{ return m_pCamera; }

	Vec3		GetCameraLockTargetPos();

private:
	wstring ChangeStateByKeyInfo();
	void ChangeState(wstring _sStateName);

	wstring Check_Tap();
	wstring Check_Pressed();
	wstring Check_Away();



	CLONE(CStateMgr)

public:
	CStateMgr();
	virtual ~CStateMgr();

};

