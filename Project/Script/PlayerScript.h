#pragma once
#include <Engine/CScript.h>
#include "CObjKeyMgr.h"
#include "CPlayerStat.h"

class CStateMgr;
class CObjKeyMgr;
class CCamera;


class PlayerScript
	: public CScript
{
private:
	CObjKeyMgr*		m_pKeyMgr;			//  키	정보 관리 
	CStateMgr*		m_pStateMgr;		// 상태 정보 관리
	CPlayerStat*	m_pStat;			// 스탯 정보 관리 
	CGameObject*	m_pCamera;			// 카메라 관리 


private:
	Vec3			m_vObjForwardAxis;  // 앞 방향 
	
	CGameObject*	m_pSwordCollider;	// 무기		충돌체
	CGameObject*	m_pSword_Trail;		// 무기		트레일 
	CGameObject*	m_pDirectionalLight;

public:
	virtual void start();
	virtual void update();
	virtual void lateupdate();

	virtual void OnCollisionEnter(CGameObject* _OtherObject) override;
	virtual void OnCollision(CGameObject* _OtherObject) override;
	virtual void OnCollisionExit(CGameObject* _OtherObject) override;

public:
	virtual void SaveToScene(FILE* _pFile) override;
	virtual void LoadFromScene(FILE* _pFile) override;

public:
	CObjKeyMgr*  GetObjKeyMgr()			{ return m_pKeyMgr; }
	CStateMgr*	 GetStateMgr()			{ return m_pStateMgr; }
	CPlayerStat* GetPlayerStat()		{ return m_pStat; }
	CGameObject* GetCamera()			{ return m_pCamera; }
	Vec3		 GetForwardAxis()		{ return m_vObjForwardAxis; }
	CGameObject* GetSwordBoneCollider() { return m_pSwordCollider; }
	Vec3		 GetMoveDir();


	void SetCamera(CGameObject* _pCameraObj)	{ m_pCamera = _pCameraObj; }
	void SetForwardAxis(Vec3 _vForwardAxis)		{ m_vObjForwardAxis = _vForwardAxis; }

public:
	void UpdateCamera();
	void UpdateSwordCollider();
	void UpdateDirectionalLight();


	CGameObject* GetChildObj(CGameObject* _parent, wstring _name);


	CLONE(PlayerScript)

	// 외부에서 스탯 부르는 함수 0.0 ~ 1.0f
public:
	void Stat_Up(STAT_TYPE _eType, float _fPercent);
	void Stat_Down(STAT_TYPE _eType, float _fPercent);




public:
	PlayerScript(const PlayerScript& _origin);
	PlayerScript();
	virtual ~PlayerScript();
};
