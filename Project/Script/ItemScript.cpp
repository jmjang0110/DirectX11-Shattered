#include "pch.h"
#include "ItemScript.h"

#include "PlayerScript.h"

#include <Engine/CLight3D.h>
#include <Engine/CSerializer.h>
#include <Engine/CParticleSystem.h>
#include <Engine/CCollider3D.h>


ItemScript::ItemScript()
	: CScript{ (int)SCRIPT_TYPE::ITEMSCRIPT }
	, m_eItemType(ItemType::NONE)
	, m_pOwnerObject(nullptr)
	, m_pParticleSystem(nullptr)
	, m_fColliderSphereSize(0.f)
	, m_bItemUsed(false)

{
}

ItemScript::ItemScript(const ItemScript& _origin)
	: CScript{ (int)SCRIPT_TYPE::ITEMSCRIPT }
	, m_eItemType(_origin.m_eItemType)
	, m_pOwnerObject(_origin.m_pOwnerObject)
	, m_pParticleSystem(_origin.m_pParticleSystem)
	, m_fColliderSphereSize(_origin.m_fColliderSphereSize)
	, m_bItemUsed(_origin.m_bItemUsed)

{
}

ItemScript::~ItemScript()
{
}

void ItemScript::SetItemType(ItemType _type)
{
	m_eItemType = _type;

	m_fColliderSphereSize = 20.f;
}

void ItemScript::SetParticleObject(CGameObject* _particle)
{
	m_pParticleSystem = _particle;

}

void ItemScript::start()
{
	if (m_eItemType == ItemType::ITEM_HEALING)
	{
		/*CPrefab* pPrefab = CResMgr::GetInst()->Load<CPrefab>(L"prefab\\item.pref", L"prefab\\item.pref").Get();

		CGameObject* parti = pPrefab->Instantiate();
		parti->ParticleSystem()->SetMaterial(L"material\\item.mtrl");
		GetOwner()->AddChild(parti);*/
	}
	else if (m_eItemType == ItemType::ITME_HP_POTION)
	{

	}
}

void ItemScript::update()
{
}

void ItemScript::lateupdate()
{
	if (m_bItemUsed)
	{
		if (m_eItemType == ItemType::ITEM_HEALING)
		{
			vector<CGameObject*> vec = CSceneMgr::GetInst()->GetCurScene()->GetLayer(L"BG")->GetObjects();
			for (size_t i = 0; i < vec.size(); i++)
			{
				if (L"HP Cure Effect" == vec[i]->GetName())
				{
					vec[i]->ParticleSystem()->SetAliveCount(0);
				}
			}
		}
		
	}
}

void ItemScript::OnCollisionEnter(CGameObject* _OtherObject)
{

}

void ItemScript::OnCollision(CGameObject* _OtherObject)
{
	if (nullptr != _OtherObject->GetScript<PlayerScript>())
	{
		if (m_eItemType == ItemType::ITEM_HEALING)
		{
			GetOwner()->ParticleSystem()->SetAliveCount(0);
			//_OtherObject->GetScript<PlayerScript>()->

			if (false == m_bItemUsed)
			{
				// 회복 particle 추가
				CPrefab* pPrefab = CResMgr::GetInst()->Load<CPrefab>(L"prefab\\player_cure.pref", L"prefab\\player_cure.pref").Get();
				CGameObject* pParticle = pPrefab->Instantiate();
				pParticle->SetName(L"Player Cure");
				pParticle->ParticleSystem()->SetLifeTime(3.f);
				pParticle->ParticleSystem()->SetParticlePlayOneTime();
				pParticle->ParticleSystem()->SetMaterial(L"material\\player_cure.mtrl");
				_OtherObject->AddChild(pParticle);
				m_bItemUsed = true;
			}			
		}
		else if (m_eItemType == ItemType::ITME_HP_POTION)
		{
			//_OtherObject->GetScript<PlayerScript>()->AddHPPotion()
		}

	}
}

void ItemScript::OnCollisionExit(CGameObject* _OtherObject)
{
}

void ItemScript::Serialize(YAML::Emitter& emitter)
{
	//emitter << YAML::Key << NAME_OF(m_eItemType) << YAML::Value << m_eItemType;
	//m_pOwnerObject->Serialize(emitter);
	//m_pParticleObject->Serialize(emitter);
	emitter << YAML::Key << NAME_OF(m_fColliderSphereSize) << YAML::Value << m_fColliderSphereSize;

	CScript::Serialize(emitter);
}

void ItemScript::Deserialize(const YAML::Node& node)
{
	//m_pOwnerObject->Deserialize(node);
	//m_pParticleObject->Deserialize(node);
	m_fColliderSphereSize = node[NAME_OF(m_fColliderSphereSize)].as<float>();

	CScript::Deserialize(node);
}
