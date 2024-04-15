#pragma once
#include <Engine/CScript.h>

class CStructuredBuffer;
class CAnimator3D;
class CGameObject;
class CMeshData;

struct tTrail
{
	CGameObject* pTrailObj;
	float		 fTimer;
};

class TrailScript :
	public CScript
{
private:
	CGameObject* m_pCam;

	CGameObject* m_pOriginObj;			// 잔상을 일으킨 원본 오브젝트  
	Ptr<CMeshData>			m_pOribinObj_MeshData;  // 잔상을 일으킨 원본 오브젝트의 메쉬 데이터  
	CGameObject* m_pTrail;


	float					m_fCreateTerm;			// 잔상이 일어나는 시간 간격 
	float					m_fTimer;				// 잔상이 일어난 누적 시간
	float					m_fDestroyTime;			// 잔상이 사라질 시간 ( 잔상 지속 시간 )

	bool					m_bCreateTrail;
	vector<tTrail>			m_vecTrail;				// 잔상 오브젝트 모음
	int						m_iTrailMaxCnt;			// 잔상 갯수


	bool					m_bPlayTrail;			// 잔상 출력 여부 


public:
	void SetOriginObject(CGameObject* _Origin);

	void SetTrailMeshData(wstring _strKey);
	void CreateTrail(int _FrameIdx);

	bool DeactivateTrail();
	bool ActivateTrail();
	void UpdateActiveTrail();


	void SetCreateTermTime(float _fTimer) { m_fCreateTerm = _fTimer; }
	void SetDestroyTermTime(float _fTimer) { m_fDestroyTime = _fTimer; }

	void TimerReset() { m_fTimer = 0.f; }
	int GetTrailFrameIdx();

	void On() { m_bPlayTrail = true; }
	void Off() { m_bPlayTrail = false; }


	CGameObject* SetTrailObj();
	CGameObject* CloneTrailObj();



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

	CLONE(TrailScript)

public:
	TrailScript(const TrailScript& _origin);
	TrailScript();
	virtual ~TrailScript();

};

