#pragma once
#include <Engine/CMgrScript.h>

enum class MAP_TYPE
{
	_01,
	_02,

	END
};

class CObjectManager final
	: public CMgrScript<CObjectManager>
{
private:
	CGameObject* m_pPlayer;
	CGameObject* m_pPlayerCamera;
	CGameObject* m_pBossCombatMgr;

	std::vector<CGameObject*> m_vecDonDestroy;

	std::array<Vec3, (UINT)MAP_TYPE::END> m_arrStartingPoint;

	std::map<MAP_TYPE, std::vector<CGameObject*>> m_mapSceneObject;
	MAP_TYPE                                      m_currentMap;

	CGameObject* m_pBossUI    = nullptr;
	CGameObject* m_pPlayerUI  = nullptr;
	CGameObject* m_pGate      = nullptr;
	CGameObject* m_pCinemaCam = nullptr;

public:
	CGameObject* GetPlayer() const { return m_pPlayer; }
	CGameObject* GetPlayerCamera() const { return m_pPlayerCamera; }
	CGameObject* GetBossCombatMgr() const { return m_pBossCombatMgr; }
	CGameObject* GetPlayerUI() const { return m_pPlayerUI; }
	CGameObject* GetBossUI() const { return m_pBossUI; }
	CGameObject* GetGate() const { return m_pGate; }
	CGameObject* GetCinemaCam() const { return m_pCinemaCam; }

	void SetCinemaCam(CGameObject* _pGameObject) { m_pCinemaCam = _pGameObject; }
	void SetGate(CGameObject* _pGameObject) { m_pGate = _pGameObject; }
	void SetPlayerUI(CGameObject* _pGameObject) { m_pPlayerUI = _pGameObject; }
	void SetBossUI(CGameObject* _pGameObject) { m_pBossUI = _pGameObject; }
	void SetBossCombatMgr(CGameObject* _pMgr) { m_pBossCombatMgr = _pMgr; }
	void SetPlayer(CGameObject* _pObj) { m_pPlayer = _pObj; }
	void SetPlayerCam(CGameObject* _pObj) { m_pPlayerCamera = _pObj; }

	void                             RemoveFromDontDestroyList(CGameObject* pGameObject);
	CGameObject*                     IsInDontDestroyList(const CGameObject* pGameObject);
	void                             AddToDontDestroy(CGameObject* pGameObject);
	const std::vector<CGameObject*>& GetDontDestroyList() const { return m_vecDonDestroy; }

	Vec3 GetStartingPoint(MAP_TYPE _type) const { return m_arrStartingPoint[static_cast<UINT>(_type)]; }

	MAP_TYPE GetCurrentMapType() const { return m_currentMap; }
	void     SetMapType(MAP_TYPE _type) { m_currentMap = _type; }

	void                             SetSceneObject(CGameObject* pGameObject, MAP_TYPE type);
	const std::vector<CGameObject*>& GetSceneObjectList(MAP_TYPE _type) const;

	bool CheckAllMonsterDead() const;

public:
	void CreateAttackCollider(float _lifeTime, float _sphereSize, Vec3 _pos);

public:
	void start() override;

public:
	CObjectManager();
	virtual ~CObjectManager();
};
