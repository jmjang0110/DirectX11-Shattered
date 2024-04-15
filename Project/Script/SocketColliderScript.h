#pragma once
#include <Engine/CScript.h>

class SocketColliderScript :
    public CScript
{
private:
	CGameObject* m_pAnimObj;			// Animation 을 갖고있는 Object;
	CGameObject* m_pColliderObj;		// Socket Collider GameObject 


	int			m_iSocketIdx;			// Socket 인덱스 
	Matrix		m_SocketMatrix;			// Socket 행렬정보
	Vec3		m_vSocketRot;			// Socket 회전 
	Vec3		m_vSocketPos;			// Socket 위치 

	Vec3		m_vSocket_OffsetPos;	// Socket 오프셋 위치 

public:
	virtual void start();
	virtual void update();
	virtual void lateupdate();

	virtual void OnCollisionEnter(CGameObject* _OtherObject) override;
	virtual void OnCollision(CGameObject* _OtherObject) override;
	virtual void OnCollisionExit(CGameObject* _OtherObject) override;

public:
	// [ SET PART ]
	void SetSocketIdx(int _idx) { m_iSocketIdx = _idx; }
	void SetSocketMartix(Matrix _Mat) { m_SocketMatrix = _Mat; }
	void SetSocketRotation(Vec3 _vRot) { m_vSocketRot = _vRot; }
	void SetSocketPosition(Vec3 _vPos) { m_vSocketPos = _vPos; }
	void SetSocketOffsetPos(Vec3 _vPos) { m_vSocket_OffsetPos = _vPos; }
	
	void SetAnimObj(CGameObject* _pObj)		{ m_pAnimObj = _pObj; }
	void SetColliderObj(CGameObject* _pObj) { m_pColliderObj = _pObj; }

	// [ GET PART ]
	int		GetSocketIdx()		{ return m_iSocketIdx; }
	Matrix	GetSocketMatrix()	{ return m_SocketMatrix; }
	Vec3	GetSocketRotation() { return m_vSocketRot; }
	Vec3	GetSocketPosition() { return m_vSocketPos; }
	Vec3	GetSocketOffsetPos() { return m_vSocket_OffsetPos; }

	int GetvecSocketMaxSize();



public:
	virtual void SaveToScene(FILE* _pFile) override;
	virtual void LoadFromScene(FILE* _pFile) override;

	CLONE(SocketColliderScript)
public:
	SocketColliderScript();
	SocketColliderScript(const SocketColliderScript& _origin);
	virtual ~SocketColliderScript();

};
