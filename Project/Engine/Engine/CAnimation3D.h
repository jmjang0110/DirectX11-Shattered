#pragma once
#include "CEntity.h"

class CAnimator3D;

enum class ANIMATION_STATE
{
	BEFORE_PLAY,	// 재생 이전 
	PLAY,			// 재생 
	STOP,			// 일시 정지 
	FINISH,			// 끝 

	END
};

class CAnimation3D
	: public CEntity
{
private:
	CAnimator3D* m_pOwner;
	tMTAnimClip  m_tClip;				// 애니메이션 정보  

	ANIMATION_STATE m_eCurState;			// 현재 애니메이션 상태 

	int m_iFrameCnt;			// 30 프레임 - Default 

	bool m_bFinalMatUpdate;		// 최종행렬 연산 수행여부
	bool m_bFinish;				// 애니메이션 종료 여부
	bool m_bPlay;				// 애니메이션 실행 여부 

	// [ CLIP INFO	] 
	int m_iCurClip;				// 클립 번호 ( Idx )
	// [ FRAME INFO ]
	int   m_iPrevAnimEndFrameIdx; // 이전 애니메이션 끝 프레임 번호
	int   m_iCurFrameIdx;			// 현재 프레임 번호
	int   m_iNextFrameIdx;		// 다음 프레임 번호 
	float m_fSpeed;				// 프레임 속도 
	float m_fRatio;				// 프레임 - 프레임 사이 시간에 따른 비율 
	// [ TIME INFO ]
	vector<double> m_vecClipUpdateTime;	// 애니메이션 플레이 이후 누적시간 
	double         m_dCurTime;				// 현재 시간 
	float          m_fLerpTime;			// 보간 시간 

	float m_fLerpTimeAcc;
	int   m_iPlayCnt;			// 애니메이션이 몇번 반복 실행되고 있는지 카운트

	bool m_hasCallback = false;
	bool m_fired       = false;

	std::unordered_map<int, std::function<void()>> m_mapCallback;

public:
	void SetCallback(std::function<void()> func, int frameIndex);
	void FireCallback(int frameIndex);

public:
	void finalupdate();
	void UpdateData() override;
	void ClearData() const;

public:
	void Play(bool _bEnable);

	void Reset();
	void ResetStartEndFrameIdx();
	void ResetStartEndFrameTime();
	void CopyInfo(CAnimation3D** _pCopyAnim) const; // Copy될 Animation의 이중포인터를 넘겨라 


public:
	// [ SET PART ]
	void SetOwner(CAnimator3D* _owner) { m_pOwner = _owner; }
	void SetFrameInfoByFrame(int _ClipNum, int _startFrameIdx = 0, int _EndFrameIdx = 0);
	void SetFrameInfoByTime(int _ClipNum, double _StartTime = 0.0, double _EndTime = 0.0);

	void SetClipUpdateTime(int _vecClipSize);
	void SetPrevFrameEndIdx(int _idx) { m_iPrevAnimEndFrameIdx = _idx; }
	void CopyClipUpdateTime(vector<double> _vecClipUpdateTime) { m_vecClipUpdateTime = _vecClipUpdateTime; }


	// - SET CLIP INFO 
	void SetClipInfo(tMTAnimClip _tClip) { m_tClip = _tClip; }
	void SetClipNum(int _ClipNum) { m_iCurClip = _ClipNum; }
	void SetStartTime(double _startTime);
	void SetEndTime(double _endTime);
	void SetStartFrameIdx(int _startIdx);
	void SetEndFrameIdx(int _endIdx);
	void SetCurFrameIdx(int _curIdx);

	void SetSpeed(float _fSpeed) { m_fSpeed = _fSpeed; }
	void SetFinish(bool _bEnable);
	void SetPlay(bool _bEnable);

	void SetAnimState(ANIMATION_STATE _eState) { m_eCurState = _eState; }
	void SetLerpTime(float _fTime) { m_fLerpTime = _fTime; }

public:
	// [ GET PART ]
	bool            IsPlay() const { return m_bPlay; }
	bool            IsFinish() const { return m_bFinish; }
	int             GetClipNum() const { return m_iCurClip; }
	float           GetSpeed() const { return m_fSpeed; }
	ANIMATION_STATE GetState() const { return m_eCurState; }
	// - GET INDEX -
	int GetCurFrameIdx() const { return m_iCurFrameIdx; }
	int GetStartFrameIdx() const { return m_tClip.iStartFrame; }
	int GetEndFrameIdx() const { return m_tClip.iEndFrame; }
	int GetMaxFrameIdx() const;
	int GetPlayCnt() const { return m_iPlayCnt; }


	// - GET TIME -
	float  GetLerpTime() const { return m_fLerpTime; }
	double GetStartTime() const { return m_tClip.dStartTime; }
	double GetEndTime() const { return m_tClip.dEndTime; }
	double GetAccTime() const { return m_vecClipUpdateTime[m_iCurClip]; }

private:
	void check_mesh(Ptr<CMesh> _pMesh) const;

public:
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(CAnimation3D);
	CAnimation3D();
	CAnimation3D(const CAnimation3D& _origin);
	virtual ~CAnimation3D();

	friend class CAnimator3D;
};
