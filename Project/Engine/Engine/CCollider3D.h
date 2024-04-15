#pragma once
#include "CComponent.h"

enum class COLLIDER3D_TYPE
{
	CUBE,
	SPHERE
};

class CCollider3D final
	: public CComponent
{
private:
	Vec3            m_vOffsetPos;
	Vec3            m_vOffsetScale;
	int             m_iCollisionCount;
	COLLIDER3D_TYPE m_eCollider3DType;
	Matrix          m_matWorld;

	Ptr<CMesh>     m_pMesh;
	Ptr<CMaterial> m_pMaterial;

	float m_fLifeTime;

public:
	void SetCollider3DType(COLLIDER3D_TYPE _type);
	void SetOffsetPos(Vec3 _vOffsetPos);
	void SetOffsetScale(Vec3 _vOffsetScale);

	COLLIDER3D_TYPE GetCollider3DType() const;
	Vec3            GetWorldPos() const;
	Vec3            GetWorldScale() const;
	Matrix          GetWorldMatrix() const;

	//Editor¿ë
	Vec3 GetOffsetPos() const;
	Vec3 GetOffsetScale() const;

public:
	void OnCollisionEnter(CCollider3D* _pOther);
	void OnCollision(CCollider3D* _pOther);
	void OnCollisionExit(CCollider3D* _pOther);

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	void SetLifeTime(float _time) { m_fLifeTime = _time; }

public:
	void UpdateData() override;
	void finalupdate() override;
	void finalupdate_debug() override;

	void render();
	void render_debug() override;

public:
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(CCollider3D);
	CCollider3D();
	CCollider3D(const CCollider3D& _origin);
	virtual ~CCollider3D();
};
