#pragma once
#include "CEntity.h"

class CMRT
	: public CEntity
{
private:
	Ptr<CTexture> m_arrRT[8];
	int           m_iRTCount;

	Ptr<CTexture> m_pDSTex;
	Vec4          m_arrClearColor[8];

	D3D11_VIEWPORT m_tViewPort;

public:
	void Create(int _iRenderTargetCount, Ptr<CTexture>* _ppTex, Ptr<CTexture> _pDSTex);

	const D3D11_VIEWPORT& GetViewPort() const { return m_tViewPort; }
	Ptr<CTexture>         GetArrRt(int _arrIDX) { return m_arrRT[_arrIDX]; }
	void                  SetClearColor(int _iCount, const Vec4* _pColor);
	int                   GetRenderTargetCount() const { return m_iRTCount; }

	void OMSet();
	void Clear();

public:
	CLONE_DISABLE(CMRT);

public:
	CMRT();
	virtual ~CMRT();
};
