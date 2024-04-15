#pragma once
#include "CComponent.h"

class CRigidBody
	: public CComponent
{
private:
	Vec3 m_vForce;           // 크기 , 방향 
	Vec3 m_vAccel;           // 가속도  
	Vec3 m_vAccelA;          // 추가 가속도 
	Vec3 m_vVelocity;        // 속도 ( 크기 : 속력 , 방향 )

	float m_fGravityAccelAlpha;	// 낙하 속도  
	float m_fMass;            // 질량 
	float m_fFricCoeff;       // 마찰 계수 
	float m_vAccelRatio;      // Accel 적용 비율 

	float m_fMaxMoveSpeed;
	float m_fMaxJumpSpeed;    // 점프 최대 속도
	float m_fMinJumpSpeed;	// 점프 최소 속도

	float m_fCurVelocity_MoveSpeed;
	float m_fCurVelocity_JumpSpeed;

	bool m_bIgnoreGravity;   // 중력 영향 여부 
	bool m_bOnGround;		// 땅에 착지했는지 여부  

	Vec3 m_vMovedDist;		// 움직인 거리 


public:
	void AddForce(Vec3 _vF) { m_vForce += _vF; }
	void AddVelocity(Vec3 _v) { m_vVelocity += _v; }
	void AddAccelAlpha(Vec3 _vAccelA) { m_vAccelA += _vAccelA; }

	bool IsOnGround() const { return m_bOnGround; }
	void SetOnGround(bool _b) { m_bOnGround = _b; }

	Vec3 GetMoveDist(Vec3 _vDist) { return m_vMovedDist; }

public:
	void  SetForce(Vec3 _vForce) { m_vForce = _vForce; }
	void  SetVelocity(Vec3 _v) { m_vVelocity = _v; }		// 초기속도를 빠르게 합니다. 
	void  SetMass(float _fMass) { m_fMass = _fMass; }
	void  SetFricCoeff(float _fFricCoeff) { m_fFricCoeff = _fFricCoeff; }
	void  SetAccelAlpha(Vec3 _vAccelA) { m_vAccelA = _vAccelA; }
	void  SetMaxSpeed(float _fMaxSpeed) { m_fMaxMoveSpeed = _fMaxSpeed; }
	void  SetIgnoreGravity(bool _b) { m_bIgnoreGravity = _b; }
	void  SetGravityAccleAlpha(float _fGravityAccelAlpha) { m_fGravityAccelAlpha = _fGravityAccelAlpha; }
	Vec3  GetForce() { return m_vForce; }
	Vec3  GetVelocity() { return m_vVelocity; }
	Vec3  GetAccelAlpha() { return m_vAccelA; }
	float GetMass() { return m_fMass; }
	float GetFricCoeff() { return m_fFricCoeff; }

public:
	void update() override;
	void lateupdate() override;
	void finalupdate() override;
	void finalupdate_debug() override;
	void render();

public:
	void UpdateGravity();
	Vec3 UpdateVelocity();							// 속도   업데이트 
	void UpdateFriction(Vec3 _vVelocityDir);							// 마찰력 업데이트
	void UpdateLimitSpeed(Vec3 _vVelocityDir);		// 이동/점프/낙하 속도 제한 
	void Move(Vec3 _vVelocityDir);	// 이동 
	void Jump(Vec3 _vVelocityDir);	// 점프/낙하 


public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;
	CLONE(CRigidBody);

public:
	void Serialize(YAML::Emitter& emitter) override;;
	void Deserialize(const YAML::Node& node) override;

public:
	CRigidBody();
	CRigidBody(const CRigidBody& _origin);
	virtual ~CRigidBody();
};
