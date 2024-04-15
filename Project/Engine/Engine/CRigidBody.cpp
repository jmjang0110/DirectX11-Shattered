#include "pch.h"
#include "CRigidBody.h"


#include "CTimeMgr.h"
#include "CTransform.h"
#include "CSerializer.h"

CRigidBody::CRigidBody()
	: CComponent(COMPONENT_TYPE::RIGIDBODY)
	, m_vForce(Vec3(0.f, 0.f, 0.f))
	, m_vAccel(Vec3(0.f, 0.f, 0.f))
	, m_vAccelA(Vec3(0.f, 0.f, 0.f))
	, m_vVelocity(Vec3(0.f, 0.f, 0.f))
	, m_fGravityAccelAlpha(-2000.f)
	, m_fMass(1.f)
	, m_fFricCoeff(500.f)
	, m_vAccelRatio(1.f)
	, m_fMaxMoveSpeed(200.f)
	, m_fMaxJumpSpeed(1200.f)
	, m_fMinJumpSpeed(-550.f)
	, m_fCurVelocity_MoveSpeed{0}
	, m_fCurVelocity_JumpSpeed{0}
	, m_bIgnoreGravity{false}
	, m_bOnGround{false} {}

CRigidBody::CRigidBody(const CRigidBody& _origin)
	: CComponent(_origin)
	, m_vForce(_origin.m_vForce)
	, m_vAccel(_origin.m_vAccel)
	, m_vAccelA(_origin.m_vAccelA)
	, m_vVelocity(_origin.m_vVelocity)
	, m_fGravityAccelAlpha(_origin.m_fGravityAccelAlpha)
	, m_fMass(_origin.m_fMass)
	, m_fFricCoeff(_origin.m_fFricCoeff)
	, m_vAccelRatio(_origin.m_vAccelRatio)
	, m_fMaxMoveSpeed(_origin.m_fMaxMoveSpeed)
	, m_fMaxJumpSpeed(_origin.m_fMaxJumpSpeed)
	, m_fMinJumpSpeed(_origin.m_fMinJumpSpeed)
	, m_fCurVelocity_MoveSpeed{0}
	, m_fCurVelocity_JumpSpeed{0}
	, m_bIgnoreGravity{false}
	, m_bOnGround{false} {}


CRigidBody::~CRigidBody() = default;


void CRigidBody::update()
{
	Vec3 vPrevPos = Transform()->GetRelativePos();		// 이전 위치 

	{
		UpdateGravity();
		Vec3 vNormalized_VelocityDir = UpdateVelocity();	// 속도 업데이트 
		UpdateFriction(vNormalized_VelocityDir);			// 마찰력 적용 
		UpdateLimitSpeed(vNormalized_VelocityDir);			// 속도제한 업데이트 	

		Move(vNormalized_VelocityDir);						// (앞뒤좌우) 이동 
		Jump(vNormalized_VelocityDir);						// (상하)     이동 

		m_vForce  = Vec3(0.f, 0.f, 0.f);						// 힘			초기화 
		m_vAccel  = Vec3(0.f, 0.f, 0.f);						// 가속도		초기화 
		m_vAccelA = Vec3(0.f, 0.f, 0.f);					// 추가 가속도	초기화
	}

	Vec3 vCurPos = Transform()->GetRelativePos();		// 현재 위치 
	m_vMovedDist = vPrevPos - vCurPos; 					// 이동 거리 

	// TEST : 일단 0.f 아래로 안떨어지게 막아둠 - Nav Mesh 후 수정할 예정 
	//Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	//if (vPos.y < 0.f)
	//{
	//	vPos.y = 0.f;
	//	GetOwner()->Transform()->SetRelativePos(vPos);
	//	m_bOnGround = true;
	//}
	//else
	//	m_bOnGround = false;
}

void CRigidBody::UpdateGravity()
{
	// 땅 위에 있지 않다면 
	if (!m_bOnGround)
	{
		if (!m_bIgnoreGravity)
		{
			// 떨어지는 가속도 세팅 -> m_fMinJUmpSpeed 에 의해 속도 제한 걸림 
			SetAccelAlpha(Vec3(0.f, m_fGravityAccelAlpha, 0.f));
		}
	}
	else
	{
		m_vVelocity.y = 0.f;
	}
}

Vec3 CRigidBody::UpdateVelocity()
{
	if (0.f != m_vForce.Length())
	{
		// Velocity 
		m_vAccel = m_vForce / m_fMass;	// 무거울 수록 힘이 약해진다. 
	}

	m_vAccel += m_vAccelA;				// 추가 가속도 ex) 중력
	m_vVelocity += m_vAccel * DT;		// 가속도에 따른 속도 변화 

	if (m_bIgnoreGravity)
		m_vVelocity.y = 0.f;			// 중력 무시  ( 떨어지거나 올라가지 않음 ) 

	Vec3 vNormalized_VelocityDir = m_vVelocity;					// 방향 
	vNormalized_VelocityDir.Normalize();

	return vNormalized_VelocityDir;
}


void CRigidBody::UpdateFriction(Vec3 _vVelocityDir)
{
	if (m_vVelocity != Vec3(0.f, 0.f, 0.f))
	{
		Vec3 vFricDir  = -1 * _vVelocityDir;
		Vec3 vFriction = vFricDir * m_fFricCoeff * DT;  // 마찰력 계산

		if (m_vVelocity.Length() < vFriction.Length())
		{
			m_vVelocity = Vec3(0.f, 0.f, 0.f);			// 마찰력에 의한 정지 
		}
		else
		{
			m_vVelocity += vFriction;					// 마찰력 적용  
		}
	}
}

void CRigidBody::UpdateLimitSpeed(Vec3 _vVelocityDir)
{
	m_fCurVelocity_MoveSpeed = m_vVelocity.Length();		// X,Z	스피드
	m_fCurVelocity_JumpSpeed = m_vVelocity.y;			// Y	스피드 


	// 점프 일 때 속도 제한 
	if (m_vVelocity.y >= 0.f)
	{
		// 점프 속도 제한 
		if (m_fCurVelocity_JumpSpeed >= m_fMaxJumpSpeed)
			m_fCurVelocity_JumpSpeed = m_fMaxJumpSpeed;
	}
	// 앞 뒤 좌 우 방향 속도 제한 
	else
	{
		// 속도 제한 
		if (m_fCurVelocity_MoveSpeed >= m_fMaxMoveSpeed)
			m_fCurVelocity_MoveSpeed = m_fMaxMoveSpeed;
	}

	// 떨어지는 속도 제한
	if (m_fCurVelocity_JumpSpeed <= m_fMinJumpSpeed)
	{
		m_fCurVelocity_JumpSpeed = m_fMinJumpSpeed;
	}
}

void CRigidBody::Move(Vec3 _vVelocityDir)
{
	// Move 
	if (0.f != m_fCurVelocity_MoveSpeed)
	{
		Vec3 vDir = m_vVelocity;
		vDir.Normalize();								// 이동 방향 

		Vec3 vPos = Transform()->GetRelativePos();
		vPos.x += _vVelocityDir.x * m_fCurVelocity_MoveSpeed * DT;
		vPos.z += _vVelocityDir.z * m_fCurVelocity_MoveSpeed * DT;
		Transform()->SetRelativePos(vPos);
	}
}


void CRigidBody::Jump(Vec3 _vVelocityDir)
{
	// Jump
	if (0.f != m_fCurVelocity_JumpSpeed)
	{
		Vec3 vDir = _vVelocityDir;										// 이동 방향 

		Vec3 vPos = Transform()->GetRelativePos();

		if (vDir.y >= 0.f)
			vPos.y += _vVelocityDir.y * m_fCurVelocity_JumpSpeed * DT;		// JUMP
		else
			vPos.y += _vVelocityDir.y * m_fCurVelocity_JumpSpeed * -1 * DT;	// FALL

		Transform()->SetRelativePos(vPos);
	}
}

void CRigidBody::lateupdate() {}

void CRigidBody::finalupdate() {}

void CRigidBody::finalupdate_debug() {}

void CRigidBody::render() {}

void CRigidBody::SaveToScene(FILE* _pFile) {}

void CRigidBody::LoadFromScene(FILE* _pFile) {}

void CRigidBody::Serialize(YAML::Emitter& emitter)
{
	emitter << YAML::Key << "RIGIDBODY INFO" << YAML::Value << YAML::Flow << YAML::BeginMap;

	emitter << YAML::Key << NAME_OF(m_vForce) << YAML::Value << m_vForce;
	emitter << YAML::Key << NAME_OF(m_vAccel) << YAML::Value << m_vAccel;
	emitter << YAML::Key << NAME_OF(m_vAccelA) << YAML::Value << m_vAccelA;
	emitter << YAML::Key << NAME_OF(m_vVelocity) << YAML::Value << m_vVelocity;

	emitter << YAML::Key << NAME_OF(m_fGravityAccelAlpha) << YAML::Value << m_fGravityAccelAlpha;
	emitter << YAML::Key << NAME_OF(m_fMass) << YAML::Value << m_fMass;
	emitter << YAML::Key << NAME_OF(m_fFricCoeff) << YAML::Value << m_fFricCoeff;
	emitter << YAML::Key << NAME_OF(m_vAccelRatio) << YAML::Value << m_vAccelRatio;


	emitter << YAML::Key << NAME_OF(m_fMaxMoveSpeed) << YAML::Value << m_fMaxMoveSpeed;
	emitter << YAML::Key << NAME_OF(m_fMaxJumpSpeed) << YAML::Value << m_fMaxJumpSpeed;
	emitter << YAML::Key << NAME_OF(m_fMinJumpSpeed) << YAML::Value << m_fMinJumpSpeed;

	emitter << YAML::Key << NAME_OF(m_fCurVelocity_MoveSpeed) << YAML::Value << m_fCurVelocity_MoveSpeed;
	emitter << YAML::Key << NAME_OF(m_fCurVelocity_JumpSpeed) << YAML::Value << m_fCurVelocity_JumpSpeed;

	emitter << YAML::Key << NAME_OF(m_bIgnoreGravity) << YAML::Value << m_bIgnoreGravity;
	emitter << YAML::Key << NAME_OF(m_bOnGround) << YAML::Value << m_bOnGround;
}


void CRigidBody::Deserialize(const YAML::Node& node)
{
	YAML::Node RigidBodyInfoNode = node["RIGIDBODY INFO"];

	m_vForce    = RigidBodyInfoNode[NAME_OF(m_vForce)].as<Vec3>();
	m_vAccel    = RigidBodyInfoNode[NAME_OF(m_vAccel)].as<Vec3>();
	m_vAccelA   = RigidBodyInfoNode[NAME_OF(m_vAccelA)].as<Vec3>();
	m_vVelocity = RigidBodyInfoNode[NAME_OF(m_vVelocity)].as<Vec3>();

	m_fGravityAccelAlpha = RigidBodyInfoNode[NAME_OF(m_fGravityAccelAlpha)].as<float>();
	m_fMass              = RigidBodyInfoNode[NAME_OF(m_fMass)].as<float>();
	m_fFricCoeff         = RigidBodyInfoNode[NAME_OF(m_fFricCoeff)].as<float>();
	m_vAccelRatio        = RigidBodyInfoNode[NAME_OF(m_vAccelRatio)].as<float>();

	m_fMaxMoveSpeed = RigidBodyInfoNode[NAME_OF(m_fMaxMoveSpeed)].as<float>();
	m_fMaxJumpSpeed = RigidBodyInfoNode[NAME_OF(m_fMaxJumpSpeed)].as<float>();
	m_fMinJumpSpeed = RigidBodyInfoNode[NAME_OF(m_fMinJumpSpeed)].as<float>();

	m_fCurVelocity_MoveSpeed = RigidBodyInfoNode[NAME_OF(m_fCurVelocity_MoveSpeed)].as<float>();
	m_fCurVelocity_JumpSpeed = RigidBodyInfoNode[NAME_OF(m_fCurVelocity_JumpSpeed)].as<float>();

	m_bIgnoreGravity = RigidBodyInfoNode[NAME_OF(m_bIgnoreGravity)].as<bool>();
	m_bOnGround      = RigidBodyInfoNode[NAME_OF(m_bOnGround)].as<bool>();
}
