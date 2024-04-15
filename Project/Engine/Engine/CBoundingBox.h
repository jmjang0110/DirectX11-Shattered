#pragma once
#include "CComponent.h"

class CBoundingBox : public CComponent
{
private:
	Vec4 m_vColor;
	Vec3 m_vOffsetPos;
	Vec3 m_vOffsetScale;

	Matrix         m_matWorld;
	Ptr<CMesh>     m_pMesh;
	Ptr<CMaterial> m_pMaterial;

	bool m_bBoundCheck;

public:
	void SetBoundCheck(bool _bCheck) { m_bBoundCheck = _bCheck; }

public:
	void SetOffsetPos(Vec3 _vOffsetPos);
	void SetOffsetScale(Vec3 _vOffsetScale);
	void SetColor(Vec4 vColor);


	Vec3   GetWorldPos() const;
	Vec3   GetWorldScale() const;
	Matrix GetWorldMatrix() const;

	//Editor¿ë
	Vec3 GetOffsetPos() const;
	Vec3 GetOffsetScale() const;

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
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	CLONE(CBoundingBox);
	CBoundingBox();
	CBoundingBox(const CBoundingBox& _origin);
	virtual ~CBoundingBox();
};
