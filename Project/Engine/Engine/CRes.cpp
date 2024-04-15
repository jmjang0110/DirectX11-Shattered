#include "pch.h"
#include "CRes.h"

CRes::CRes(RES_TYPE _eType)
	: m_eResType(_eType)
	, m_iRefCount(0)
	, m_bEngineRes(false)
	, m_bChanged(false) {}

CRes::~CRes() = default;

bool CRes::CheckFail(HRESULT _hr)
{
	if (FAILED(_hr))
	{
		LOG_WARN("LOADING TEXTURE HAS AN ISSUE : %d", GetLastError());
		return true;
	}
	return false;
}
