#pragma once
#include "CEntity.h"

#include "CGameObject.h"

#define GET_OTHER_COMPONENT(Type) C##Type* Type() { return GetOwner()->Type(); }

class CComponent
	: public CEntity
{
public:
	static CComponent* MakeComponent(const std::wstring& name);

private:
	const COMPONENT_TYPE m_eComType;
	CGameObject*         m_pOwner;

	bool m_bActive;

protected:
	CGameObject* m_pDebugObj;

public:
	virtual void start() { }
	virtual void update() { }
	virtual void lateupdate() { }
	virtual void finalupdate() = 0;
	virtual void finalupdate_debug() { };
	virtual void finalupdate_module() { };
	virtual void render_debug() { };

	void Activate();
	void Deactivate();

	bool IsActive() const { return m_bActive; }

	void         SetDebugObj(CGameObject* _pObj) { m_pDebugObj = _pObj; }
	CGameObject* GetDebugObj() { return m_pDebugObj; }

	void ActiateDebugObj() const
	{
		assert(m_pDebugObj);
		m_pDebugObj->Activate();
	}

	void DeactivateDebugObj() const
	{
		assert(m_pDebugObj);
		m_pDebugObj->Deactivate();
	}

	bool IsActivateDebugObj() const
	{
		assert(m_pDebugObj);
		return m_pDebugObj->IsActive();
	}

public:
	COMPONENT_TYPE GetType() const { return m_eComType; }
	CGameObject*   GetOwner() const { return m_pOwner; }

	GET_OTHER_COMPONENT(Transform)
	GET_OTHER_COMPONENT(MeshRender)
	GET_OTHER_COMPONENT(Camera)
	GET_OTHER_COMPONENT(Collider2D)
	GET_OTHER_COMPONENT(Animator2D)
	GET_OTHER_COMPONENT(Animator3D)
	GET_OTHER_COMPONENT(TileMap)
	GET_OTHER_COMPONENT(ParticleSystem)
	GET_OTHER_COMPONENT(SkyBox)
	GET_OTHER_COMPONENT(Light2D)
	GET_OTHER_COMPONENT(Light3D)
	GET_OTHER_COMPONENT(Decal)
	GET_OTHER_COMPONENT(LandScape)
	GET_OTHER_COMPONENT(Collider3D);
	GET_OTHER_COMPONENT(BoundingBox);
	GET_OTHER_COMPONENT(FSM);
	GET_OTHER_COMPONENT(RigidBody);
	GET_OTHER_COMPONENT(NaviMap);
	GET_OTHER_COMPONENT(NaviAgent);
	GET_OTHER_COMPONENT(UIImage);
	GET_OTHER_COMPONENT(UIButton);
	GET_OTHER_COMPONENT(UIPanel);
	GET_OTHER_COMPONENT(UIProgressBar);
	GET_OTHER_COMPONENT(UIText);

	CComponent* Clone() override = 0;

protected:
	virtual void active() { m_bActive = true; };
	virtual void deactive() { m_bActive = false; };

public:
	CComponent(COMPONENT_TYPE _eType);
	virtual ~CComponent();

	friend class CGameObject;
	friend class CEventMgr;
};
