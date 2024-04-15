#include "PathMgr.h"

void CPathMgr::Init()
{
	const std::filesystem::path debug_path = std::filesystem::current_path();
	solution_path                          = debug_path.parent_path().parent_path();
	content_path                           = solution_path / "OutputFile" / "bin" / "content" / "";
	include_path                           = solution_path / "External" / "Include" / "";
	project_path                           = solution_path / "Project" / "";
}

std::wstring CPathMgr::SolutionPath()
{
	std::wstring path = solution_path / "";
	return path;
}

std::wstring CPathMgr::ContentPath()
{
	return content_path.wstring();
}

std::wstring CPathMgr::IncludePath()
{
	return include_path.wstring();
}

std::wstring CPathMgr::ProjectPath()
{
	return project_path.wstring();
}

std::wstring CPathMgr::GetFileName(const std::wstring& fileName)
{
	return std::filesystem::path{fileName}.filename().wstring();
}

std::wstring CPathMgr::GetExtension(const std::wstring& filePath)
{
	return std::filesystem::path{filePath}.extension().wstring();
}

std::wstring CPathMgr::GetRelativePath(const std::wstring& fullPath)
{
	const size_t pos = fullPath.find_last_of(content_path.wstring());

	if (pos != fullPath.npos)
	{
		return fullPath.substr(pos);
	}

	return std::wstring{};
}
