#pragma once
#include <filesystem>
#include <string>

class CPathMgr
{
private:
	inline static std::filesystem::path solution_path;
	inline static std::filesystem::path content_path;
	inline static std::filesystem::path include_path;
	inline static std::filesystem::path project_path;

public:
	static void         Init();
	static std::wstring GetFileName(const std::wstring& fileName);
	static std::wstring GetExtension(const std::wstring& filePath);
	static std::wstring GetRelativePath(const std::wstring& fullPath);

	static std::wstring SolutionPath();
	static std::wstring ContentPath();
	static std::wstring IncludePath();
	static std::wstring ProjectPath();

private:
	CPathMgr()  = default;
	~CPathMgr() = default;
};
