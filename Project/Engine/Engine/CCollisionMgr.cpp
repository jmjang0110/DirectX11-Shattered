#include "pch.h"
#include "CCollisionMgr.h"

#include "CResMgr.h"

#include "CSceneMgr.h"
#include "CScene.h"
#include "CLayer.h"

#include "CGameObject.h"
#include "CCollider2D.h"
#include "CCollider3D.h"


CCollisionMgr::CCollisionMgr()
	: m_arrCheck{} {}

CCollisionMgr::~CCollisionMgr() = default;

void CCollisionMgr::update()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		for (UINT j = i; j < MAX_LAYER; ++j)
		{
			if (m_arrCheck[i] & (1 << j))
			{
				// i 에 해당하는 EOBJ_TYPE 오브젝트들과, j 에 해당하는EOBJ_TYPE 오브젝트들 끼리 충돌 진행
				const CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
				CollisionBetweenLayer(pCurScene->GetLayer(i)->GetObjects(), pCurScene->GetLayer(j)->GetObjects());
				CollisionBetween3D(pCurScene->GetLayer(i)->GetObjects(), pCurScene->GetLayer(j)->GetObjects());
			}
		}
	}
}

void CCollisionMgr::CollisionBetweenLayer(const vector<CGameObject*>& _left, const vector<CGameObject*>& _right)
{
	for (auto& pLeftGameObject : _left)
	{
		CCollider2D* pLeftCol = pLeftGameObject->Collider2D();
		if (nullptr == pLeftCol)
		{
			continue;
		}

		for (auto& pRightGameObject : _right)
		{
			CCollider2D* pRightCol = pRightGameObject->Collider2D();
			if (nullptr == pRightCol)
			{
				continue;
			}

			//자기 자신인 경우
			if (pLeftCol == pRightCol)
			{
				continue;
			}

			// 두 충돌체가 이전에 충돌했었는지 확인
			// 두 충돌체의 조합 아이디 생성
			COLLIDER_ID ID{};
			ID.iLeftID  = pLeftCol->GetID();
			ID.iRightID = pRightCol->GetID();
			auto iter   = m_mapColInfo.find(ID.id);

			// 두 충돌체는 처음 검사를 진행하고 있다.
			if (iter == m_mapColInfo.end())
			{
				m_mapColInfo.insert({ID.id, false});
				iter = m_mapColInfo.find(ID.id);
			}

			// 두 충돌체중 하나라도 데드 상태인지 
			const bool bAnyDead = pLeftCol->GetOwner()->IsDead() || pRightCol->GetOwner()->IsDead();

			// 두 충돌체(혹은 게임오브젝트) 중 하나라도 비활성화 상태인지
			const bool bAnyDeActive = !pLeftCol->GetOwner()->IsActive()
			                          || !pRightCol->GetOwner()->IsActive()
			                          || !pLeftCol->IsActive()
			                          || !pRightCol->IsActive();

			// 이전 프레임에서는 충돌하지 않고 있었고, 현재 둘중 하나 이상이 비활성화 상태이면 충돌 검사를 하지 않겠다.
			if (bAnyDeActive && false == iter->second)
			{
				continue;
			}

			// 충돌 검사
			// 충돌 중이다.
			if (IsCollision(pLeftCol, pRightCol))
			{
				// 이전 프레임에도 충돌 중이었다.
				if (iter->second)
				{
					// 충돌 중이다
					if (bAnyDead || bAnyDeActive)
					{
						// 둘중 하나라도 삭제 예정이라면(삭제를 통한 충돌 해제가 발생하는 것으로 본다)
						pLeftCol->OnCollisionExit(pRightCol);
						pRightCol->OnCollisionExit(pLeftCol);

						if (bAnyDead)
							m_mapColInfo.erase(iter);
						else
							iter->second = false;
					}
					else
					{
						pLeftCol->OnCollision(pRightCol);
						pRightCol->OnCollision(pLeftCol);
					}
				}

				// 이전에는 충돌하지 않고 있었다.
				else
				{
					// 이번에 막 충돌 진입했다.
					if (!bAnyDead)
					{
						// 둘 중 하나라도 삭제 예정이라면 충돌 시작 자체를 하지 않게 한다.
						pLeftCol->OnCollisionEnter(pRightCol);
						pRightCol->OnCollisionEnter(pLeftCol);
						iter->second = true;
					}
				}
			}

			// 충돌하지 않고 있다.
			else
			{
				// 이전 프레임에서는 충돌 중이었다.
				if (iter->second)
				{
					pLeftCol->OnCollisionExit(pRightCol);
					pRightCol->OnCollisionExit(pLeftCol);
					iter->second = false;
				}
			}
		}
	}
}


bool CCollisionMgr::IsCollision(CCollider2D* _pLeftCol, CCollider2D* _pRightCol)
{
	if (_pLeftCol->GetCollider2DType() == COLLIDER2D_TYPE::BOX
	    && _pRightCol->GetCollider2DType() == COLLIDER2D_TYPE::BOX)
	{
		return IsCollision_Box(_pLeftCol, _pRightCol);
	}
	if (_pLeftCol->GetCollider2DType() == COLLIDER2D_TYPE::CIRCLE
	    && _pRightCol->GetCollider2DType() == COLLIDER2D_TYPE::CIRCLE)
	{
		return IsCollision_Circle(_pLeftCol, _pRightCol);
	}
	return false;
}

bool CCollisionMgr::IsCollision_Box(CCollider2D* _pLeftCol, CCollider2D* _pRightCol)
{
	// 충돌체가 사용하는 기본 도형(사각형) 로컬 정점위치를 알아낸다.
	// 0 -- 1
	// | \  |
	// 3 -- 2	
	static CMesh*     pRectMesh    = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh").Get();
	const static Vtx* pVtx         = pRectMesh->GetVtxSysMem();
	const static Vec3 vLocalPos[4] = {pVtx[0].vPos, pVtx[1].vPos, pVtx[2].vPos, pVtx[3].vPos};

	const Matrix matLeft  = _pLeftCol->GetWorldMat();
	const Matrix matRight = _pRightCol->GetWorldMat();

	// Local 스페이스의 네개의 정점을 각 충돌체 월드 위치로 보낸다.
	Vec3 arrAxis[4] = {};

	// 월드로 보낸 정점을 통해서 각 투영 축 이면서 투영시킬 벡터 성분 4개를 구한다.
	arrAxis[0] = XMVector3TransformCoord(vLocalPos[1], matLeft) - XMVector3TransformCoord(vLocalPos[0], matLeft);
	arrAxis[1] = XMVector3TransformCoord(vLocalPos[3], matLeft) - XMVector3TransformCoord(vLocalPos[0], matLeft);
	arrAxis[2] = XMVector3TransformCoord(vLocalPos[1], matRight) - XMVector3TransformCoord(vLocalPos[0], matRight);
	arrAxis[3] = XMVector3TransformCoord(vLocalPos[3], matRight) - XMVector3TransformCoord(vLocalPos[0], matRight);

	// 월드에 배치된 두 충돌체의 중심을 이은 벡터
	const Vec3 centerVector = _pRightCol->GetWorldPos() - _pLeftCol->GetWorldPos();

	for (size_t i = 0; i < std::size(arrAxis); ++i)
	{
		arrAxis[i].z           = 0.f;
		Vec3 axisForProjection = arrAxis[i].Normalize();

		float distance = 0.f;
		for (size_t j = 0; j < std::size(arrAxis); ++j)
		{
			// projectedVector 에 arrAxis[j] 를 투영시킨 길이		
			distance += abs(arrAxis[j].Dot(axisForProjection));
		}

		distance *= 0.5f;
		const float centerDistance = abs(centerVector.Dot(axisForProjection));
		if (distance < centerDistance)
		{
			return false;
		}
	}

	return true;
}

bool CCollisionMgr::IsCollision_Circle(const CCollider2D* _pLeftCol, const CCollider2D* _pRightCol)
{
	const Vec3  centerVector = _pLeftCol->GetWorldPos() - _pRightCol->GetWorldPos();
	const float distance     = centerVector.Length();
	const float radius       = fabsf(_pLeftCol->GetWorldScale().x) * 0.5f + fabsf(_pRightCol->GetWorldScale().x) * 0.5f;

	if (radius < distance)
	{
		return false;
	}

	return true;
}

void CCollisionMgr::CollisionBetween3D(const std::vector<CGameObject*>& _vecLeft,
                                       const std::vector<CGameObject*>& _vecRight)
{
	for (auto& pLeftGameObject : _vecLeft)
	{
		CCollider3D* pLeftCol = pLeftGameObject->Collider3D();

		if (nullptr == pLeftCol)
		{
			continue;
		}

		for (auto& pRightGameObject : _vecRight)
		{
			CCollider3D* pRightCol = pRightGameObject->Collider3D();

			if (nullptr == pRightCol)
			{
				continue;
			}

			//자기 자신인 경우
			if (pLeftCol == pRightCol)
			{
				continue;
			}

			// 두 충돌체가 이전에 충돌했었는지 확인
			// 두 충돌체의 조합 아이디 생성
			COLLIDER_ID ID{};
			ID.iLeftID  = pLeftCol->GetID();
			ID.iRightID = pRightCol->GetID();
			auto iter   = m_mapColInfo.find(ID.id);

			// 두 충돌체는 처음 검사를 진행하고 있다.
			if (iter == m_mapColInfo.end())
			{
				m_mapColInfo.insert({ID.id, false});
				iter = m_mapColInfo.find(ID.id);
			}

			// 두 충돌체중 하나라도 데드 상태인지 
			const bool bAnyDead = pLeftCol->GetOwner()->IsDead() || pRightCol->GetOwner()->IsDead();

			// 두 충돌체(또는 게임오브젝트) 중 하나라도 비활성화 상태인지
			const bool bAnyDeActive = !pLeftCol->GetOwner()->IsActive()
			                          || !pRightCol->GetOwner()->IsActive()
			                          || !pLeftCol->IsActive()
			                          || !pRightCol->IsActive();

			// 이전 프레임에서는 충돌하지 않고 있었고, 현재 둘중 하나 이상이 비활성화 상태이면 충돌 검사를 하지 않겠다.
			if (bAnyDeActive && false == iter->second)
			{
				continue;
			}

			// 충돌 검사
			// 충돌 중이다.
			if (IsCollision3D(pLeftCol, pRightCol))
			{
				// 이전 프레임에도 충돌 중이었다.
				if (iter->second)
				{
					// 충돌 중이다
					if (bAnyDead || bAnyDeActive)
					{
						// 둘중 하나라도 삭제 예정이라면(삭제를 통한 충돌 해제가 발생하는 것으로 본다)
						pLeftCol->OnCollisionExit(pRightCol);
						pRightCol->OnCollisionExit(pLeftCol);

						if (bAnyDead)
							m_mapColInfo.erase(iter);
						else
							iter->second = false;
					}
					else
					{
						pLeftCol->OnCollision(pRightCol);
						pRightCol->OnCollision(pLeftCol);
					}
				}
				// 이전에는 충돌하지 않고 있었다.
				else
				{
					// 이번에 막 충돌 진입했다.
					if (!bAnyDead)
					{
						// 둘 중 하나라도 삭제 예정이라면 충돌 시작 자체를 하지 않게 한다.
						pLeftCol->OnCollisionEnter(pRightCol);
						pRightCol->OnCollisionEnter(pLeftCol);
						iter->second = true;
					}
				}
			}
			// 충돌하지 않고 있다.
			else
			{
				// 이전 프레임에서는 충돌 중이었다.
				if (iter->second)
				{
					pLeftCol->OnCollisionExit(pRightCol);
					pRightCol->OnCollisionExit(pLeftCol);
					iter->second = false;
				}
			}
		}
	}
}

void CCollisionMgr::CollisionCheck(int _iLayerLeftIdx, int _iLayerRightIdx)
{
	UINT iCol{};
	UINT iRow{};

	if (static_cast<UINT>(_iLayerLeftIdx) > static_cast<UINT>(_iLayerRightIdx))
	{
		iCol = static_cast<UINT>(_iLayerLeftIdx);
		iRow = static_cast<UINT>(_iLayerRightIdx);
	}
	else
	{
		iCol = static_cast<UINT>(_iLayerRightIdx);
		iRow = static_cast<UINT>(_iLayerLeftIdx);
	}

	m_arrCheck[iRow] |= (1 << iCol);
}

void CCollisionMgr::CollisionOff(int _iLayerLeftIdx, int _iLayerRightIdx)
{
	UINT iCol{};
	UINT iRow{};

	if (static_cast<UINT>(_iLayerLeftIdx) > static_cast<UINT>(_iLayerRightIdx))
	{
		iCol = static_cast<UINT>(_iLayerLeftIdx);
		iRow = static_cast<UINT>(_iLayerRightIdx);
	}
	else
	{
		iCol = static_cast<UINT>(_iLayerRightIdx);
		iRow = static_cast<UINT>(_iLayerLeftIdx);
	}

	m_arrCheck[iRow] &= ~(1 << iCol);
}

void CCollisionMgr::CollisionCheck(const wstring& _strLeftName, const wstring& _strRightName)
{
	const CScene* pCurScene   = CSceneMgr::GetInst()->GetCurScene();
	const CLayer* pLeftLayer  = pCurScene->GetLayer(_strLeftName);
	const CLayer* pRightLayer = pCurScene->GetLayer(_strRightName);

	if (pLeftLayer && pRightLayer)
	{
		CollisionCheck(pLeftLayer->GetLayerIdx(), pRightLayer->GetLayerIdx());
	}
}

void CCollisionMgr::CollisionOff(const wstring& _strLeftName, const wstring& _strRightName)
{
	const CScene* pCurScene   = CSceneMgr::GetInst()->GetCurScene();
	const CLayer* pLeftLayer  = pCurScene->GetLayer(_strLeftName);
	const CLayer* pRightLayer = pCurScene->GetLayer(_strRightName);

	if (pLeftLayer && pRightLayer)
	{
		CollisionOff(pLeftLayer->GetLayerIdx(), pRightLayer->GetLayerIdx());
	}
}

bool CCollisionMgr::IsCollision3D(const CCollider3D* _pLeft, const CCollider3D* _pRight)
{
	if (_pLeft->GetCollider3DType() == COLLIDER3D_TYPE::CUBE
	    && _pRight->GetCollider3DType() == COLLIDER3D_TYPE::CUBE)
	{
		return IsCollisionCube(_pLeft, _pRight);
	}

	if (_pLeft->GetCollider3DType() == COLLIDER3D_TYPE::SPHERE
	    && _pRight->GetCollider3DType() == COLLIDER3D_TYPE::SPHERE)
	{
		return IsCollisionSphere(_pLeft, _pRight);
	}

	if ((_pLeft->GetCollider3DType() == COLLIDER3D_TYPE::SPHERE
	     && _pRight->GetCollider3DType() == COLLIDER3D_TYPE::CUBE)
	    || (_pLeft->GetCollider3DType() == COLLIDER3D_TYPE::CUBE
	        && _pRight->GetCollider3DType() == COLLIDER3D_TYPE::SPHERE))
	{
		return IsCollisionSphereWithCube(_pLeft, _pRight);
	}
	return false;
}

bool CCollisionMgr::IsCollisionCube(const CCollider3D* _pLeftCol, const CCollider3D* _pRightCol)
{
	//두 육면체의 중심 사이의 거리
	const Vec3            vCenter    = _pRightCol->GetWorldPos() - _pLeftCol->GetWorldPos();
	const tCollider3DInfo tLeftInfo  = ExtractColliderInfo(_pLeftCol);
	const tCollider3DInfo tRightInfo = ExtractColliderInfo(_pRightCol);

	// leftInfo 시점
	if (ExistSAT(vCenter, tLeftInfo.axisX, tLeftInfo, tRightInfo)) return false;
	if (ExistSAT(vCenter, tLeftInfo.axisY, tLeftInfo, tRightInfo)) return false;
	if (ExistSAT(vCenter, tLeftInfo.axisZ, tLeftInfo, tRightInfo)) return false;

	// rightInfo 시점
	if (ExistSAT(vCenter, tRightInfo.axisX, tLeftInfo, tRightInfo)) return false;
	if (ExistSAT(vCenter, tRightInfo.axisY, tLeftInfo, tRightInfo)) return false;
	if (ExistSAT(vCenter, tRightInfo.axisZ, tLeftInfo, tRightInfo)) return false;

	// top, side view 시점
	if (ExistSAT(vCenter, tLeftInfo.axisX.Cross(tRightInfo.axisX), tLeftInfo, tRightInfo)) return false;
	if (ExistSAT(vCenter, tLeftInfo.axisX.Cross(tRightInfo.axisY), tLeftInfo, tRightInfo)) return false;
	if (ExistSAT(vCenter, tLeftInfo.axisX.Cross(tRightInfo.axisZ), tLeftInfo, tRightInfo)) return false;
	if (ExistSAT(vCenter, tLeftInfo.axisY.Cross(tRightInfo.axisX), tLeftInfo, tRightInfo)) return false;
	if (ExistSAT(vCenter, tLeftInfo.axisY.Cross(tRightInfo.axisY), tLeftInfo, tRightInfo)) return false;
	if (ExistSAT(vCenter, tLeftInfo.axisY.Cross(tRightInfo.axisZ), tLeftInfo, tRightInfo)) return false;
	if (ExistSAT(vCenter, tLeftInfo.axisZ.Cross(tRightInfo.axisX), tLeftInfo, tRightInfo)) return false;
	if (ExistSAT(vCenter, tLeftInfo.axisZ.Cross(tRightInfo.axisY), tLeftInfo, tRightInfo)) return false;
	if (ExistSAT(vCenter, tLeftInfo.axisZ.Cross(tRightInfo.axisZ), tLeftInfo, tRightInfo)) return false;

	return true;
}

bool CCollisionMgr::IsCollisionSphere(const CCollider3D* _pLeftCol, const CCollider3D* _pRightCol)
{
	//실제 지정은 지름 크기로 되므로, 0.5f를 곱해주므로써, 반지름 길이로 얻도록 보정.
	const float fLeftRadius     = _pLeftCol->GetWorldScale().x * .5f;
	const float fRightRadius    = _pRightCol->GetWorldScale().x * .5f;
	const float fCenterDistance = Vec3::Distance(_pLeftCol->GetWorldPos(), _pRightCol->GetWorldPos());

	if (fCenterDistance < fLeftRadius + fRightRadius)
	{
		return true;
	}
	return false;
}

bool CCollisionMgr::IsCollisionSphereWithCube(const CCollider3D* _pLeftCol, const CCollider3D* _pRightCol)
{
	////한쪽 구를 큐브로 얅게 감사서 큐브라고 가정하여 분리축 검사를 먼저 진행한다.
	//if (IsCollisionCube(_pLeftCol, _pRightCol))

	const CCollider3D* pBox    = (_pLeftCol->GetCollider3DType() == COLLIDER3D_TYPE::CUBE) ? _pLeftCol : _pRightCol;
	const CCollider3D* pCircle = (_pLeftCol == pBox) ? _pRightCol : _pLeftCol;

	const Vec3& closestPoint  = ClosedPointInOBB(pBox, pCircle->GetWorldPos());
	const float distanceSqure = (closestPoint - pCircle->GetWorldPos()).LengthSquared();
	//0.5f를 해주는 이유는 반지름은 로컬에서 0.5f이므로, 우리가 실제 배율한 것보다 절반이다.
	const float radiusSqured = (pCircle->GetWorldScale().x * .5f) * (pCircle->GetWorldScale().x * .5f);

	if (distanceSqure < radiusSqured)
	{
		return true;
	}
	return false;
}

bool CCollisionMgr::ExistSAT(const Vec3&            _vCenter,
                             const Vec3&            _vTarget,
                             const tCollider3DInfo& _tLeftInfo,
                             const tCollider3DInfo& _tRightInfo)
{
	const float centerProjected = fabsf(_vCenter.Dot(_vTarget));

	float distance = 0.f;
	distance += fabsf((_tLeftInfo.axisX * _tLeftInfo.halfScale.x).Dot(_vTarget));
	distance += fabsf((_tLeftInfo.axisY * _tLeftInfo.halfScale.y).Dot(_vTarget));
	distance += fabsf((_tLeftInfo.axisZ * _tLeftInfo.halfScale.z).Dot(_vTarget));

	distance += fabsf((_tRightInfo.axisX * _tRightInfo.halfScale.x).Dot(_vTarget));
	distance += fabsf((_tRightInfo.axisY * _tRightInfo.halfScale.y).Dot(_vTarget));
	distance += fabsf((_tRightInfo.axisZ * _tRightInfo.halfScale.z).Dot(_vTarget));

	if (distance < centerProjected)
	{
		return true;
	}
	return false;
}

tCollider3DInfo CCollisionMgr::ExtractColliderInfo(const CCollider3D* _pCollider)
{
	// 큐브를 이루는 정점
	static Vec3 arrLocal[8] =
	{
		// 윗면
		Vec3(-0.5f, 0.5f, 0.5f),	// 0
		Vec3(0.5f, 0.5f, 0.5f),		// 1
		Vec3(0.5f, 0.5f, -0.5f),	// 2
		Vec3(-0.5f, 0.5f, -0.5f),	// 3

		// 아랫면
		Vec3(-0.5f, -0.5f, -0.5f), //4
		Vec3(0.5f, -0.5f, -0.5f),	//5
		Vec3(0.5f, -0.5f, 0.5f),	//6
		Vec3(-0.5f, -0.5f, 0.5f)	//7
	};

	//const Vec3 vertexPos_0 = XMVector3TransformCoord(arrLocal[0], _pCollider->GetWorldMatrix());
	//const Vec3 vertexPos_1 = XMVector3TransformCoord(arrLocal[1], _pCollider->GetWorldMatrix());
	//const Vec3 vertexPos_3 = XMVector3TransformCoord(arrLocal[3], _pCollider->GetWorldMatrix());
	//const Vec3 vertexPos_7 = XMVector3TransformCoord(arrLocal[7], _pCollider->GetWorldMatrix());

	//tCollider3DInfo info{};
	////월드 상에서의 각 축의 위치벡터
	//info.axisX = (vertexPos_1 - vertexPos_0).Normalize();
	//info.axisY = (vertexPos_3 - vertexPos_0).Normalize();
	//info.axisZ = (vertexPos_7 - vertexPos_0).Normalize();

	tCollider3DInfo info{};
	info.axisX = _pCollider->GetWorldMatrix().Right().Normalize();
	info.axisY = _pCollider->GetWorldMatrix().Up().Normalize();
	info.axisZ = _pCollider->GetWorldMatrix().Forward().Normalize();

	// 각 육면체의 중심부터 변까지의 거리
	info.centerPos = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), _pCollider->GetWorldMatrix());
	info.centerPos.Normalize();
	info.halfScale = _pCollider->GetWorldScale() * .5f;
	return info;
}

Vec3 CCollisionMgr::ClosedPointInOBB(const CCollider3D* _pCollider, const Vec3& _point)
{
	const Vec3&   direction = _point - _pCollider->GetWorldPos();
	const Vec3&   scale     = _pCollider->GetWorldScale() * 0.5f; //0.5f를 해주는 이유는 로컬에서 0.5f로 잡았으므로, 실제 우리가 배율한 것보다 절반이다.
	const Matrix& matOBB    = _pCollider->GetWorldMatrix();
	//각 축으로 Projection하여 확인
	const Vec3& axisX     = matOBB.Right().Normalize();
	const float distanceX = ClampData(direction.Dot(axisX), -scale.x, scale.x);
	const Vec3& axisY     = matOBB.Up().Normalize();
	const float distanceY = ClampData(direction.Dot(axisY), -scale.y, scale.y);
	const Vec3& axisZ     = matOBB.Forward().Normalize();
	const float distanceZ = ClampData(direction.Dot(axisZ), -scale.z, scale.z);

	//Adjust the final _point by the axis and the current projected distance:
	const Vec3 result = _pCollider->GetWorldPos() + axisX * distanceX + axisY * distanceY + axisZ * distanceZ;

	return result;
}
