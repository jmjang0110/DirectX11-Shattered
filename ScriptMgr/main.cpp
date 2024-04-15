#include "PathMgr.h"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include <vector>
#include <format>
#include <unordered_set>

std::vector<std::wstring> vecWorkingScriptList{};
std::vector<std::wstring> vecExternalScriptList{};
std::vector<std::wstring> vecUpperName{};

int main()
{
	CPathMgr::Init();

	std::wstring projectPath          = CPathMgr::ProjectPath();
	std::wstring scriptManager_cpp    = projectPath + L"Script\\CScriptMgr.cpp";
	std::wstring scriptManager_header = projectPath + L"Script\\CScriptMgr.h";

	std::unordered_set<std::wstring> setExcept{};
	std::wifstream                   inputFile_except{"ExceptList.txt"};
	if (inputFile_except.is_open())
	{
		std::wstring scriptList{};
		inputFile_except.seekg(0, std::ios::end);
		size_t fileSize = inputFile_except.tellg();
		scriptList.resize(fileSize);
		inputFile_except.seekg(0, std::ios::beg);

		inputFile_except.read(scriptList.data(), scriptList.size());

		std::wstringstream ss{scriptList};
		std::wstring       scriptName{};

		while (std::getline(ss, scriptName, L'\n'))
		{
			std::erase_if(scriptName, ::isspace);
			std::erase_if(scriptName, [](const wchar_t ch) { return ch == L'\0'; });
			if (false == scriptName.empty())
			{
				setExcept.insert(scriptName);
			}
		}
	}

	std::wstring includePath   = CPathMgr::IncludePath();
	std::wstring strScriptCode = includePath + L"Script\\";

	std::filesystem::path scriptFolder = includePath + L"Script";

	for (const auto& entry : std::filesystem::directory_iterator{scriptFolder})
	{
		if (entry.path().extension() == ".h")
		{
			std::wstring fileName = entry.path().filename();

			if (setExcept.contains(fileName))
			{
				continue;
			}

			vecWorkingScriptList.push_back(entry.path().stem());
		}
	}

	std::wifstream inputFile{"ScriptList.txt"};
	if (inputFile.is_open())
	{
		std::wstring scriptList{};
		inputFile.seekg(0, std::ios::end);
		size_t fileSize = inputFile.tellg();
		scriptList.resize(fileSize);
		inputFile.seekg(0, std::ios::beg);

		inputFile.read(scriptList.data(), scriptList.size());

		std::wstringstream ss{scriptList};
		std::wstring       scriptName{};

		while (std::getline(ss, scriptName, L'\n'))
		{
			std::erase_if(scriptName, ::isspace);
			std::erase_if(scriptName, [](const wchar_t ch) { return ch == L'\0'; });
			if (false == scriptName.empty())
			{
				vecExternalScriptList.push_back(scriptName);
			}
		}
	}
	inputFile.close();

	if (vecWorkingScriptList == vecExternalScriptList)
	{
		return 0;
	}

	std::wstringstream script_header_content{};

	script_header_content << LR"(
#pragma once

#include <vector>
#include <string>
using namespace std;

enum class SCRIPT_TYPE
{)";
	for (const auto& scriptName : vecWorkingScriptList)
	{
		std::wstring upperName = scriptName.substr(1);
		std::ranges::transform(upperName, upperName.begin(), towupper);
		script_header_content << std::format(L"\t{},\n", upperName);

		vecUpperName.push_back(upperName);
	}
	script_header_content << LR"(
    END
};)";

	script_header_content << LR"(

class CScript;

class CScriptMgr
{
public:
    static void GetScriptInfo(vector<wstring>& _vec);
    static CScript* GetScript(const wstring& _strScriptName);
    static CScript* GetScript(UINT _iScriptType);
    static const wchar_t* GetScriptName(CScript* _pScript);
};
)";

	std::wfstream output_header{scriptManager_header, std::ios::out | std::ios::trunc};
	if (output_header.is_open())
	{
		output_header << script_header_content.str();
	}
	output_header.close();

	std::wstringstream script_cpp_content{};
	script_cpp_content << LR"(
#include "pch.h"

#include "CScriptMgr.h"
)";

	for (const auto& scriptName : vecWorkingScriptList)
	{
		script_cpp_content << std::format(L"#include \"{}.h\"\n", scriptName);
	}
	script_cpp_content << LR"(
void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
)";

	for (auto& scriptName : vecWorkingScriptList)
	{
		script_cpp_content << std::format(L"\t_vec.push_back(L\"{}\");\n", scriptName);
	}
	script_cpp_content << L"}\n\n";
	script_cpp_content << LR"(
CScript* CScriptMgr::GetScript(const wstring& _strScriptName)
{
)";
	for (auto& scriptName : vecWorkingScriptList)
	{
		script_cpp_content << std::format(L"\tif (L\"{}\"== _strScriptName)\n", scriptName);
		script_cpp_content << std::format(L"\t\treturn new {};\n", scriptName);
	}
	script_cpp_content << L"\treturn nullptr;\n}\n\n";

	script_cpp_content << LR"(
CScript* CScriptMgr::GetScript(UINT _iScriptType)
{
    switch((SCRIPT_TYPE)_iScriptType)
    {
)";
	for (int i = 0; i < vecWorkingScriptList.size(); ++i)
	{
		script_cpp_content << std::format(L"\tcase SCRIPT_TYPE::{}:\n", vecUpperName[i]);
		script_cpp_content << std::format(L"\t\treturn new {};\n", vecWorkingScriptList[i]);
	}
	script_cpp_content << L"\t}\n";
	script_cpp_content << L"\treturn nullptr;\n}\n\n";

	script_cpp_content << LR"(
const wchar_t* CScriptMgr::GetScriptName(CScript* _pScript)
{
    switch((SCRIPT_TYPE)_pScript->GetScriptType())
    {
)";
	for (int i = 0; i < vecWorkingScriptList.size(); ++i)
	{
		script_cpp_content << std::format(L"\tcase SCRIPT_TYPE::{}:\n", vecUpperName[i]);
		script_cpp_content << std::format(L"\t\treturn L\"{}\";\n", vecWorkingScriptList[i]);
	}
	script_cpp_content << L"\t\t}\n";
	script_cpp_content << L"\treturn nullptr;\n}\n\n";

	std::wfstream outputFile_cpp{scriptManager_cpp, std::ios::out | std::ios::trunc};
	if (outputFile_cpp.is_open())
	{
		outputFile_cpp << script_cpp_content.str();
	}
	outputFile_cpp.close();

	std::wfstream outputFile{L"Scriptst.txt", std::ios::out | std::ios::trunc};
	for (auto& scriptName : vecWorkingScriptList)
	{
		outputFile << scriptName << "\n";
	}
	outputFile.close();

	return 0;
}
