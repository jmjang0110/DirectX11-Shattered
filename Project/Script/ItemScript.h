#pragma once
#include <Engine/CScript.h>

class CGameObject;
class CParticleSystem;

enum class ItemType
{
	ITEM_HEALING,
	ITME_HP_POTION,

	NONE
};

class ItemScript :
	public CScript
{
private:
	ItemType m_eItemType;

	CGameObject* m_pOwnerObject;
	CGameObject* m_pParticleSystem;

	float m_fColliderSphereSize;
	bool m_bItemUsed;


public:
	void SetItemType(ItemType _type);
	ItemType GetItemType() { return m_eItemType; }

	void SetParticleObject(CGameObject* _particle);
	CGameObject* GetParticleObject() { return m_pParticleSystem; }
	CGameObject* GetOwnerObject() { return m_pOwnerObject; }

	float GetItemColliderSize() { return m_fColliderSphereSize; }

public:
	virtual void start();
	virtual void update();
	virtual void lateupdate();

	virtual void OnCollisionEnter(CGameObject* _OtherObject) override;
	virtual void OnCollision(CGameObject* _OtherObject) override;
	virtual void OnCollisionExit(CGameObject* _OtherObject) override;

public:
	CLONE(ItemScript)

		void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	ItemScript(const ItemScript& _origin);
	ItemScript();
	virtual ~ItemScript();
};

