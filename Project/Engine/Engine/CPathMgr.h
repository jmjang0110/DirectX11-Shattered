#pragma once

class CPathMgr
	: public CSingleton<CPathMgr>
{
	SINGLE(CPathMgr);
private:
	std::wstring m_strContentPath;

public:
	void init();

public:
	const std::wstring& GetContentPath() const { return m_strContentPath; }
	std::wstring        GetRelativePath(const wstring& _strFilePath) const;
};
