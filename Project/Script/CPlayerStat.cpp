#include "pch.h"
#include "CPlayerStat.h"

#include <Engine/CFSM.h>
#include <Engine/CUIProgressBar.h>

#include "CStateMgr.h"

#include "PlayerScript.h"
#include "CObjectManager.h"

CPlayerStat::CPlayerStat()
{
	InitCurStat(1000.f, 100.f, 100.f, 200.f, 100.f, 150.f);
	InitMaxStat(1000.f, 100.f, 100.f, 300.f, 100.f, 170.f); // TEST 용 입니다. 
}

CPlayerStat::CPlayerStat(const CPlayerStat& _origin)
	: m_tCurStat(_origin.m_tCurStat)
{
}

CPlayerStat::~CPlayerStat()
{
}


void CPlayerStat::start()
{
}

void CPlayerStat::update()
{
}

void CPlayerStat::lateupdate()
{
}

float CPlayerStat::GetDamage()
{
	PlayerScript* pPlayerScript = (PlayerScript*)m_pOwnerScript;
	wstring       strStateType  = pPlayerScript->GetStateMgr()->GetCurstate();


	float fMinAttack = 0.f;
	float fMaxAttack = m_tMaxStat.fAttack;

	if (pPlayerScript)
	{
		// 강 공격
		if (strStateType == L"HEAVT_ATTACK")
		{
			fMinAttack = m_tCurStat.fAttack;			// 200
			fMaxAttack = m_tMaxStat.fAttack + 100.f;	// 400
		}
		// 약 공격 
		else if (strStateType == L"LIGHT_ATTACK")
		{
			fMinAttack = m_tCurStat.fAttack - 100.f;	// 100
			fMaxAttack = m_tMaxStat.fAttack;			// 300
		}
	}

	float fRandDamage = float(rand() % ((int)fMaxAttack - (int)fMinAttack));
	float fDamage     = fMinAttack + fRandDamage;

	return fDamage;
}


void CPlayerStat::InitCurStat(float _fHp, float _fStamina, float _fEther, float _fAttack, float _fArmor, float _fSpeed)
{
	m_tCurStat.fHp      = _fHp;
	m_tCurStat.fStamina = _fStamina;
	m_tCurStat.fEther   = _fEther;
	m_tCurStat.fAttack  = _fAttack;
	m_tCurStat.fArmor   = _fArmor;
	m_tCurStat.fSpeed   = _fSpeed;
}

void CPlayerStat::InitMaxStat(float _fHp, float _fStamina, float _fEther, float _fAttack, float _fArmor, float _fSpeed)
{
	m_tMaxStat.fHp      = _fHp;
	m_tMaxStat.fStamina = _fStamina;
	m_tMaxStat.fEther   = _fEther;
	m_tMaxStat.fAttack  = _fAttack;
	m_tMaxStat.fArmor   = _fArmor;
	m_tMaxStat.fSpeed   = _fSpeed;
}


void CPlayerStat::SubHp(float _fHp)
{
	m_tCurStat.fHp -= _fHp;
	if (m_tCurStat.fHp <= 0.f)
		m_tCurStat.fHp = 0.f;

	CObjectManager::GetInst()->GetPlayerUI()->GetChild(L"HP_PANEL")->GetChild(L"HP_BACKGROUND")->
	                           GetChild(L"PROGRESS_BAR")->UIProgressBar()->SetPercent(m_tCurStat.fHp / m_tMaxStat.fHp);
}

void CPlayerStat::SubStamina(float _fStamina)
{
	m_tCurStat.fStamina -= _fStamina;
	if (m_tCurStat.fStamina <= 0.f)
		m_tCurStat.fStamina = 0.f;
}

void CPlayerStat::SubEther(float _fEther)
{
	m_tCurStat.fEther -= _fEther;
	if (m_tCurStat.fEther <= 0.f)
		m_tCurStat.fStamina = 0.f;
}

void CPlayerStat::SubAttack(float _fAttack)
{
	m_tCurStat.fAttack -= _fAttack;
	if (m_tCurStat.fAttack <= 0.f)
		m_tCurStat.fAttack = 0.f;
}

void CPlayerStat::SubArmor(float _fStamina)
{
	m_tCurStat.fArmor -= _fStamina;
	if (m_tCurStat.fArmor <= 0.f)
		m_tCurStat.fArmor = 0.f;
}

void CPlayerStat::AddHp(float _fHp)
{
	m_tCurStat.fHp += _fHp;
	if (m_tCurStat.fHp >= m_tMaxStat.fHp)
		m_tCurStat.fHp = m_tMaxStat.fHp;
}

void CPlayerStat::AddStamina(float _fStamina)
{
	m_tCurStat.fStamina += _fStamina;
	if (m_tCurStat.fStamina >= m_tMaxStat.fStamina)
		m_tCurStat.fStamina = m_tMaxStat.fStamina;
}

void CPlayerStat::AddEther(float _fEther)
{
	m_tCurStat.fEther += _fEther;
	if (m_tCurStat.fEther >= m_tMaxStat.fEther)
		m_tCurStat.fEther = m_tMaxStat.fEther;
}

void CPlayerStat::AddAttack(float _fAttack)
{
	m_tCurStat.fAttack += _fAttack;
	if (m_tCurStat.fAttack >= m_tMaxStat.fAttack)
		m_tCurStat.fAttack = m_tMaxStat.fAttack;
}

void CPlayerStat::AddArmor(float _fArmor)
{
	m_tCurStat.fArmor += _fArmor;
	if (m_tCurStat.fArmor >= m_tMaxStat.fArmor)
		m_tCurStat.fArmor = m_tMaxStat.fArmor;
}

void CPlayerStat::AddSpeed(float _fSpeed)
{
	m_tCurStat.fSpeed += _fSpeed;
	if (m_tCurStat.fSpeed >= m_tMaxStat.fSpeed)
		m_tCurStat.fSpeed = m_tMaxStat.fSpeed;
}

void CPlayerStat::SubSpeed(float _fSpeed)
{
	m_tCurStat.fSpeed += _fSpeed;
	if (m_tCurStat.fSpeed <= 0.f)
		m_tCurStat.fSpeed = 0.f;
}

void CPlayerStat::SaveToScene(FILE* _pFile)
{
}

void CPlayerStat::LoadFromScene(FILE* _pFile)
{
}
