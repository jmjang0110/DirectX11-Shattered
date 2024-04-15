#pragma once
#include <Engine/CEntity.h>

/*
* [ 에테르 조각 ]
		에테르 조각  은 Shattered: Tale of the Forgotten King의 소모품입니다.
		소모성 아이템은
			* HP 회복을 위한 회복 속성
			* 특정 부정적인 상태 효과 제거
			* 일시적 또는 영구적인 버프 및 디버프 적용
		같은 다양한 효과를 부여합니다.

*/

enum class STAT_TYPE
{
	HP,
	STAMINA,
	ETHER,
	ATTACK,
	ARMOR,
	SPEED,
	END,
};

struct tSTAT
{
	float	fHp;		// 체력
	float	fStamina;	// 활력 
	float	fEther;		// 에테르 

	float	fAttack;	// 공격력
	float	fArmor;		// 방어력

	float   fSpeed;		// 속도
};

class CPlayerStat :
    public CEntity
{
private:
	CScript* m_pOwnerScript;

	tSTAT	 m_tMaxStat;		// 최대 스탯
	tSTAT	 m_tCurStat;		// 현재 스탯
		
public:
	virtual void start();
	virtual void update();
	virtual void lateupdate();


	float GetDamage();


public:
	// [ S U B ]
	void SubHp(float _fHp);
	void SubStamina(float _fStamina);
	void SubEther(float _fEther);

	void SubAttack(float _fAttack);
	void SubArmor(float _fStamina);

	void SubSpeed(float _fSpeed);


	// [ A D D ]
	void AddHp(float _fHp);
	void AddStamina(float _fStamina);
	void AddEther(float _fEther);

	void AddAttack(float _fAttack);
	void AddArmor(float _fArmor);

	void AddSpeed(float _fSpeed);


public:
	// [ SET PART ]
	void InitCurStat(float _fHp = 0.f, float _fStamina = 0.f, float _fEther = 0.f,
		float _fAttack = 0.f, float _fArmor = 0.f, float _fSpeed = 0.f);
	void InitMaxStat(float _fHp = 0.f, float _fStamina = 0.f, float _fEther = 0.f,
		float _fAttack = 0.f, float _fArmor = 0.f, float _fSpeed = 0.f);
	
	void SetHP(float _fHp)				{ m_tCurStat.fHp = _fHp; }
	void SetStamina(float _fStamina)	{ m_tCurStat.fStamina = _fStamina; }
	void SetEther(float _fEther)		{ m_tCurStat.fEther = _fEther; }
	void SetAttack(float _fAttack)		{ m_tCurStat.fAttack = _fAttack; }
	void SetArmor(float _fArmor)		{ m_tCurStat.fArmor = _fArmor; }
	void SetSpeed(float _fSpeed)		{ m_tCurStat.fSpeed = _fSpeed; }

	void SetOwnerScript(CScript* _pScript) { m_pOwnerScript = _pScript; }



	// [ GET PART ]
	tSTAT GetStat() { return m_tCurStat; }
	tSTAT GetMaxStat() { return m_tMaxStat; }

public:
	virtual void SaveToScene(FILE* _pFile) override;
	virtual void LoadFromScene(FILE* _pFile) override;

	CLONE(CPlayerStat)
public:
	CPlayerStat();
	CPlayerStat(const CPlayerStat& _origin);
	virtual ~CPlayerStat();
};

