#pragma once
#include "CComponent.h"

class CNaviMapData;

class CNaviMap : public CComponent
{
private:
	Ptr<CNaviMapData> m_pNeviMapData;
	Vec4              m_vDebugMeshColor;
	Vec4              m_vDebugLineColor;

public:
	void          SetNaviMapData(CNaviMapData* _pData);
	CNaviMapData* GetNaviMapData() const { return m_pNeviMapData.Get(); }
	void          SetDebugMeshColor(Vec4 _color) { m_vDebugMeshColor = _color; }
	void          SetDebugLineColor(Vec4 _color) { m_vDebugLineColor = _color; }
	Vec4          GetDebugMeshColor() { return m_vDebugMeshColor; }
	Vec4          GetDebugLineColor() { return m_vDebugLineColor; }

	/**
	 * \brief NaviMap의 삼각형(Cell)과 충돌 테스트를 진행하는 함수
	 * \param _iCellidx NaviMap 삼각형(Cell)의 인덱스
	 * \param _vPos 대상의 World Position
	 * \param _vDir 대상 기준 충돌 방향
	 * \param _fDist 충돌 감지 거리
	 * \return 충돌 여부를 반환
	 */
	bool CheckIntersect(int _iCellidx, Vec3 _vPos, Vec3 _vDir, float& _fDist);
	void SetNavimapToAgent();

	void start() override;
	void UpdateData() override;
	void finalupdate() override;
	void finalupdate_debug() override;
	void render_debug() override;

	void active() override;
	void deactive() override;

	CLONE(CNaviMap);

	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	CNaviMap();
	CNaviMap(const CNaviMap& _origin);
	virtual ~CNaviMap();
};
