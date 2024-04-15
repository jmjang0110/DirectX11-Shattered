#include "pch.h"
#include "CPathMgr.h"

CPathMgr::CPathMgr() = default;

CPathMgr::~CPathMgr() = default;

void CPathMgr::init()
{
	const std::filesystem::path currentDirectory = std::filesystem::current_path();
	const std::filesystem::path parentDirectory  = currentDirectory.parent_path();
	m_strContentPath                             = parentDirectory.wstring() + L"\\bin\\content\\";
	LOG_TRACE("CONTENT PATH : %s", ToString(m_strContentPath).c_str());
}

std::wstring CPathMgr::GetRelativePath(const wstring& _strFilePath) const
{
	const size_t iPos = _strFilePath.rfind(m_strContentPath);
	if (iPos != _strFilePath.npos)
	{
		//iPos는 0이어야한다.(콘텐츠 폴더 경로를 시작으로 찾기 때문
		assert(iPos==0);
		return _strFilePath.substr(m_strContentPath.length());
	}
	assert(nullptr && "NOT CONTAIN RELATIVE PATH");
	return std::wstring{};
}
