#pragma once

class CGameObject;
class CCollider2D;
class CCollider3D;

union COLLIDER_ID
{
	struct
	{
		UINT iLeftID;
		UINT iRightID;
	};

	long long id;
};

struct tCollider3DInfo
{
	Vec3 axisX;
	Vec3 axisY;
	Vec3 axisZ;
	Vec3 centerPos;
	Vec3 halfScale;
};

class CCollisionMgr
	: public CSingleton<CCollisionMgr>
{
	SINGLE(CCollisionMgr);

private:
	UINT                 m_arrCheck[MAX_LAYER];
	map<long long, bool> m_mapColInfo; // 충돌 조합 고유 키

public:
	void update();
	void ClearInfo() { m_mapColInfo.clear(); }

public:
	void CollisionCheck(const wstring& _strLeftName, const wstring& _strRightName);
	void CollisionOff(const wstring& _strLeftName, const wstring& _strRightName);

	void CollisionCheck(int _iLayerLeftIdx, int _iLayerRightIdx);
	void CollisionOff(int _iLayerLeftIdx, int _iLayerRightIdx);

private:
	void CollisionBetweenLayer(const vector<CGameObject*>& _left, const vector<CGameObject*>& _right);
	bool IsCollision(CCollider2D* _pLeftCol, CCollider2D* _pRightCol);
	bool IsCollision_Box(CCollider2D* _pLeftCol, CCollider2D* _pRightCol);
	bool IsCollision_Circle(const CCollider2D* _pLeftCol, const CCollider2D* _pRightCol);

private:
	void CollisionBetween3D(const std::vector<CGameObject*>& _vecLeft, const std::vector<CGameObject*>& _vecRight);
	bool IsCollision3D(const CCollider3D* _pLeft, const CCollider3D* _pRight);
	bool IsCollisionCube(const CCollider3D* _pLeftCol, const CCollider3D* _pRightCol);
	bool IsCollisionSphere(const CCollider3D* _pLeftCol, const CCollider3D* _pRightCol);
	bool IsCollisionSphereWithCube(const CCollider3D* _pLeftCol, const CCollider3D* _pRightCol);

	//helper
	bool ExistSAT(const Vec3&            _vCenter,
	              const Vec3&            _vTarget,
	              const tCollider3DInfo& _tLeftInfo,
	              const tCollider3DInfo& _tRightInfo);
	tCollider3DInfo ExtractColliderInfo(const CCollider3D* _pCollider);
	Vec3            ClosedPointInOBB(const CCollider3D* _pCollider, const Vec3& _point);
};
