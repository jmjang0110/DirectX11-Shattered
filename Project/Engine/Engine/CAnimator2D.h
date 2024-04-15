#pragma once
#include "CComponent.h"

#include "CTexture.h"

class CAnimation2D;

class CAnimator2D : public CComponent
{
private:
	map<wstring, CAnimation2D*> m_mapAnim;
	CAnimation2D*               m_pCurAnim;
	bool                        m_bRepeat;

public:
	void        finalupdate() override;
	void        UpdateData() override;
	static void Clear();

public:
	CAnimation2D* FindAnim(const wstring& _strName);
	void          CreateAnim(const wstring& _strName,
	                         Ptr<CTexture>  _pAtlas,
	                         Vec2           _vBackgroundSizePixel,
	                         Vec2           _vLeftTopPixel,
	                         Vec2           _vSlicePixel,
	                         Vec2           _vStepPixel,
	                         float          _fDuration,
	                         int            _iFrameCount);
	void Play(const wstring& _strName, bool _bRepeat);

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(CAnimator2D);
	CAnimator2D();
	CAnimator2D(const CAnimator2D& _origin);
	virtual ~CAnimator2D();
};
