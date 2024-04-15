#include "pch.h"
#include "CEntity.h"

UINT CEntity::g_iNextID = 0;

CEntity::CEntity()
	: m_iID(g_iNextID++) {}

CEntity::CEntity(const CEntity& _origin)
	: m_iID(g_iNextID++)
	, m_strName(_origin.m_strName) {}

CEntity::~CEntity() = default;

void CEntity::SaveToScene(FILE* _pFile)
{
	SaveWStringToFile(m_strName, _pFile);
}

void CEntity::LoadFromScene(FILE* _pFile)
{
	LoadWStringFromFile(m_strName, _pFile);
}
