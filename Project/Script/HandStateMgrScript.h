#pragma once
#include <Engine/CScript.h>

class HandStateMgrScript
	:
	public CScript
{
private:
	wstring					m_sPrevState;
	wstring					m_sCurstate;
	wstring					m_sNextState;

	CGameObject*			m_pHand1;
	CGameObject*			m_pHand2;
	CGameObject*			m_pHand3;

	Vec3					m_vHand1HitBoxScale;
	Vec3					m_vHand1AttackBoxScale;

	int					m_iCurAttackHandNumber;

	bool				m_bHand01_FirstAttackDone;
	bool				m_bHand01_SecondAttackDone;


public:
	void	update() override;
	void	lateupdate() override;
	void	init();


public:
	void InitState();


public:
	 // [ SET ]
	void SetNextState(wstring _state) { m_sNextState = _state; }
	void SetCurAttackNumber(int _num) { m_iCurAttackHandNumber = _num; }
	void SetHand01_FirstAttackDone(bool _TorF) { m_bHand01_FirstAttackDone = _TorF; }
	void SetHand01_SecondAttackDone(bool _TorF) { m_bHand01_SecondAttackDone = _TorF; }

public:
	// [ GET ]
	int GetCurAttackHandNUM() { return m_iCurAttackHandNumber; }
	bool GetHand01_FirstAttackDone() { return m_bHand01_FirstAttackDone; }
	bool GetHand01_SecondAttackDone() { return m_bHand01_SecondAttackDone; }

	CGameObject* GetHandObject(int _handNum) 
	{ 
		switch (_handNum)
		{
		case 1:
			return m_pHand1;
		case 2:
			return m_pHand2;
		case 3:
			return m_pHand3;
		}
	}

	wstring GetCurState() { return m_sCurstate; }

public:
	void ResetCurAttackHandNumber();

private:
	void ChangeState(wstring _sStateName);

	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;


	CLONE(HandStateMgrScript)

public:
	HandStateMgrScript();
	virtual ~HandStateMgrScript();
};

