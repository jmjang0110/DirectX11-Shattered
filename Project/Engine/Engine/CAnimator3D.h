#pragma once
#include "CAnimation3D.h"
#include "CComponent.h"

class CStructuredBuffer;

enum class CREATE_ANIMATION_MODE
{
	FRAME,
	TIME,

	END
};

class CAnimator3D
	: public CComponent
{
private:
	// [ 3D MODEL INFO ]
	const vector<tMTBone>*		m_pVecBones;
	const vector<tMTAnimClip>*	m_pVecClip;
	int							m_iFrameCount;			 // 30
	CStructuredBuffer*			m_pBoneFinalMatBuffer;   // 특정 프레임의 최종 행렬'

	CStructuredBuffer*			m_pSocketBuffer;
	std::vector<Matrix>			m_vecSocketMatrix;
	bool						m_bUseSocket;

private:
	// [ 3D ANIMATION INFO ]
	map<wstring, CAnimation3D*> m_mapAnim;  // 전체 애니메이션 

	CAnimation3D*				m_pPrevAnim;	// 이전 애니메이션 
	CAnimation3D*				m_pCurAnim;		// 현재 애니메이션 
	CAnimation3D*				m_pNextAnim;	// 다음 애니메이션 

	bool						m_bRepeat;							// 애니메이션 반복 여부 
	bool						m_bPlayWithChild;					// 하위 객체 오브젝트와 동시에 재생 여부 

	CREATE_ANIMATION_MODE		m_eCreateMode;	// 애니메이션 생성 모드 

public:
	void finalupdate() override;
	void UpdateData() override;
	void ClearData();

public:
	CAnimation3D* FindAnim(const wstring& _strName);
	void          CopyAllAnim(map<wstring, CAnimation3D*> _mapAnim);
	void          DeleteAnim(const wstring& _wstrName);
	CAnimation3D* CreateAnimByFrame(const wstring& _strName, int _clipNum = 0, int _startFrame = 0, int _EndFrame = 0);
	CAnimation3D* CreateAnimByTime(const wstring& _strName,
	                               int            _clipNum   = 0,
	                               double         _StartTime = 0.0,
	                               double         _EndTime   = 0.0);
	void Play(const wstring& _strName, bool _bRepeat = false);
	void Clear();

	void MakeAnimationFromTXT(string _txtName);
	void MakeAnimationFromTXT_Extended(string _txtName);
	void CopyAllAnimToChild();

public:
	void SetBones(const vector<tMTBone>* _vecBones) { m_pVecBones = _vecBones; }
	void SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip);
	void SetFrameCount(int _iFrameCnt) { m_iFrameCount = _iFrameCnt; }

	void SetPlayWithChild(bool _bPlayWithChild);
	void SetAnimState(wstring _Name, ANIMATION_STATE _eState);
	void SetSpeed(float _fSpeed);
	void SetSpeedOnAnim(wstring _AnimName, float _fSpeed);
	void SetLerpTime(float _fTime);
	void SetRepeat(bool _bEnable);
	void SetLerpTimeOnAllAnim(float _fLerpTime);
	void SetLerpTimeOnAnim(wstring _AnimName, float _fLerpTime);
	void SetCreateMode(CREATE_ANIMATION_MODE _eMode) { m_eCreateMode = _eMode; }

public:
	const map<wstring, CAnimation3D*>& GetAllAnim() const { return m_mapAnim; }

	UINT        GetBoneCount() const { return static_cast<UINT>(m_pVecBones->size()); }
	tMTAnimClip GetAnimClip(int _clipIdx) const { return m_pVecClip->at(_clipIdx); }
	bool        GetRepeat() const { return m_bRepeat; }
	bool        GetPlayWithChild() const { return m_bPlayWithChild; }

	CAnimation3D* GetPrevAnim() const { return m_pPrevAnim; }
	CAnimation3D* GetCurAnim() const { return m_pCurAnim; }
	CAnimation3D* GetNextAnim() const { return m_pNextAnim; }

	int                GetFrameCount() const { return m_iFrameCount; }
	CStructuredBuffer* GetBoneFinalMatBuffer() const { return m_pBoneFinalMatBuffer; }

	CREATE_ANIMATION_MODE GetCreateMode() const { return m_eCreateMode; }
	CStructuredBuffer*    GetSocketBuffer() const { return m_pSocketBuffer; }
	int                   GetvecSocketMatSize() const { return static_cast<int>(m_vecSocketMatrix.size()); }
	void                  ResizeSocketMatrix();
	void                  DisableSocket() { m_bUseSocket = false; }
	const Matrix&         GetSocket(int index);

public:
	void RegisterPrevAnim(CAnimation3D* _pPrevAnim) { m_pPrevAnim = _pPrevAnim; }
	void RegisterNextAnim(CAnimation3D* _pNextAnim);

public:
	template <typename T, typename... Args>
	void AddCallback(const std::wstring& animationName,
	                 int                 frameIndex,
	                 T*                  pInstance,
	                 void (T::*          callback)(Args...),
	                 Args...             args);

public:
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(CAnimator3D);
	CAnimator3D();
	CAnimator3D(const CAnimator3D& _origin);
	virtual ~CAnimator3D();
};

template <typename T, typename... Args>
void CAnimator3D::AddCallback(const std::wstring& animationName,
                              int                 frameIndex,
                              T*                  pInstance,
                              void (T::*          callback)(Args...),
                              Args...             args)
{
	auto arguments = std::make_tuple(pInstance, std::forward<decltype(args)>(args)...);
	auto func      = [callback,arguments = std::move(arguments)]()
	{
		std::apply(callback, arguments);
	};

	CAnimation3D* pAnimation = FindAnim(animationName);
	LOG_ASSERT(pAnimation, "ANIMAITION NOT FOUND");

	frameIndex = ClampData(frameIndex, pAnimation->GetStartFrameIdx(), pAnimation->GetEndFrameIdx());
	pAnimation->SetCallback(std::move(func), frameIndex);
}
