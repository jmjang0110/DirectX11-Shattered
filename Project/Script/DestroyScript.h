#pragma once
#include <Engine/CScript.h>

/*
		이 스크립트를 추가하면
		지정된 시간이 지나거나
		특정 토글을 부여하면 오브젝트가 사라집니다.

*/

enum class DESTROY_MODE
{
	DEAFULT,	// 특정 타이밍 토글 사용 
	TIMER,		// 지정된 시간 
	ACTIVE,		// 아예 없어 지지 않고 DEACTIVATE

};

class DestroyScript :
	public CScript
{
private:
	float				m_fDeadLine_Timer;		// 기한 시간 
	float				m_fTimer;				// 현재 진행 시간 
	bool				m_bDestory;				// 삭제 여부 

	DESTROY_MODE		m_eDestroyMode;			// 사라지는 방법 
	float				m_fTimerRatio;


public:
	void DestroyObject();
	void SetDeadLineTime(float _fTimer) { m_fDeadLine_Timer = _fTimer; }
	void Destroy(bool _b) { m_bDestory = _b; }

	void SetDestroyMode(DESTROY_MODE _eMode) { m_eDestroyMode = _eMode; }

	void ResetTimer() { m_fTimer = 0.f; }

public:
	virtual void start();
	virtual void update();
	virtual void lateupdate();

	virtual void OnCollisionEnter(CGameObject* _OtherObject) override;
	virtual void OnCollision(CGameObject* _OtherObject) override;
	virtual void OnCollisionExit(CGameObject* _OtherObject) override;

public:
	virtual void SaveToScene(FILE* _pFile) override;
	virtual void LoadFromScene(FILE* _pFile) override;

	CLONE(DestroyScript)

public:
	DestroyScript(const DestroyScript& _origin);
	DestroyScript();
	virtual ~DestroyScript();
};

