#pragma once
#include <Engine/CScript.h>


class CGameObject;

class CGridScript : public CScript
{
private:
	CGameObject* m_pToolCam;
	Vec3         m_vToolCamPos;
	float        m_fThickness;
	float        m_fDistance;
	UINT         m_iMaxStep; // 그리드 간격 최대 단계
	float        m_fAlpha;   // 그리드 투명도
	Vec4         m_vColor;   // 그리드 색상

public:
	virtual void update();

public:
	void SetToolCamera(CGameObject* _pToolCam) { m_pToolCam = _pToolCam; }
	void SetGridColor(Vec3 _vColor) { m_vColor = _vColor; }

private:
	void CalculateGrid();

public:
	CLONE(CGridScript);

public:
	CGridScript();
	virtual ~CGridScript();
};
