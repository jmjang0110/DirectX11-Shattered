#pragma once
#include "CEntity.h"

class CCamera;

enum class PLANE
{
	PL_LEFT,
	PL_RIGHT,
	PL_UP,
	PL_DOWN,
	PL_NEAR,
	PL_FAR,
	END,
};

class CFrustum : public CEntity
{
private:
	Vec3 m_ProjPos[8];
	Vec3 m_WorldPos[8];

	Vec4 m_arrPlane[(UINT)PLANE::END];

	CCamera* m_pCam;

	bool m_bShowFrustum;

	Ptr<CMesh>     m_pMesh;
	Ptr<CMaterial> m_pMtrl;


	Matrix m_matFrustumWorld;

private:
	void CalculateFrustumMat();
	bool CheckBoundingBoxInFrustum(Vec3 _BoundingBoxCenterPos, Vec3 _BoundingBoxScale);

public:
	void UpdateData() override;
	void finalupdate();
	void render();

public:
	void SetShowFrustum(bool _bShow) { m_bShowFrustum = _bShow; }
	bool GetShowFrustum() const { return m_bShowFrustum; }

	bool PointCheck(Vec3 _vPos);
	bool SphereCheck(Vec3 _vPos, float _fRadius);

public:
	CLONE(CFrustum);
	CFrustum();
	virtual ~CFrustum();

	friend class CCamera;
};
