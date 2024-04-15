#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CMaterial.h"


enum class COLLIDER2D_TYPE
{
	BOX,
	CIRCLE,
};

class CCollider2D : public CComponent
{
private:
	Matrix m_matColWorld;

	COLLIDER2D_TYPE m_eColliderType;

	Vec2 m_vOffsetPos;
	Vec2 m_vOffsetScale;

	Ptr<CMesh>     m_pMesh;
	Ptr<CMaterial> m_pMtrl;

	int m_iCollisionCount;

public:
	void SetCollider2DType(COLLIDER2D_TYPE _type);

	void SetOffsetPos(Vec2 _vOffsetPos) { m_vOffsetPos = _vOffsetPos; }
	void SetOffsetPos(float _x, float _y) { m_vOffsetPos = Vec2(_x, _y); }

	void SetOffsetScale(Vec2 _vOffsetScale);
	void SetOffsetScale(float _x, float _y);

	COLLIDER2D_TYPE GetCollider2DType() const { return m_eColliderType; }
	Vec3            GetWorldPos() const { return m_matColWorld.Translation(); }
	Vec3            GetWorldScale() const { return Vec3(m_vOffsetScale); }
	Matrix          GetWorldMat() const { return m_matColWorld; }

public:
	void finalupdate() override;
	void UpdateData() override;
	void render();

public:
	void OnCollisionEnter(CCollider2D* _Other);
	void OnCollision(CCollider2D* _Other);
	void OnCollisionExit(CCollider2D* _Other);

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(CCollider2D);
	CCollider2D();
	CCollider2D(const CCollider2D& _Origin);
	virtual ~CCollider2D();
};
