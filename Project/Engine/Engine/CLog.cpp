#include "pch.h"
#include "CLog.h"

#if USE_THIRD_PARTY_LOGGER
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/hourly_file_sink.h>

void CLog::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	std::vector<spdlog::sink_ptr> sinks{};
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	sinks.push_back(std::make_shared<spdlog::sinks::hourly_file_sink_mt>("LOG/LOG_.txt", true));
	sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
	s_logger = std::make_shared<spdlog::logger>("ENGINE", std::begin(sinks), std::end(sinks));
	spdlog::register_logger(s_logger);
	s_logger->set_level(spdlog::level::trace);
}
#else

void CLog::Init()
{
	if (false == std::filesystem::exists("LOG"))
	{
		std::filesystem::create_directory("LOG");
	}
}

void CLog::WriteLog(LOG_LEVEL level, const char* pMessage, ...)
{
	std::lock_guard<std::mutex> lock{s_mutex};

	static const char* arrHeader[] = {"ERROR", "WARN", "INFO", "TRACE"};

	SYSTEMTIME systemtime{};
	GetLocalTime(&systemtime);
	char currentDate[32]{};
	_snprintf_s(currentDate,
	            std::size(currentDate),
	            "%d-%d-%d %d:%d:%d",
	            systemtime.wYear,
	            systemtime.wMonth,
	            systemtime.wDay,
	            systemtime.wHour,
	            systemtime.wMinute,
	            systemtime.wSecond);

	constexpr int MAX_MESSAGE_LENGTH = 2048;
	char          szBuffer[MAX_MESSAGE_LENGTH]{};

	va_list arg_list{};
	va_start(arg_list, pMessage);
	vsprintf_s(szBuffer, pMessage, arg_list);
	va_end(arg_list);

	char szFormattedBuffer[MAX_MESSAGE_LENGTH]{};
	_snprintf_s(szFormattedBuffer,
	            std::size(szFormattedBuffer),
	            "[%s] [%s] [%s] [%s]\n",
	            currentDate,
	            "ENGINE",
	            arrHeader[level],
	            szBuffer);

	//OUTPUT-DEBUG
	OutputDebugStringA(szFormattedBuffer);

	//OUTPUT-CONSOLE
	const HANDLE consoleHandle   = GetStdHandle(STD_OUTPUT_HANDLE);
	static BYTE  arrLevelColor[] =
	{
		FOREGROUND_RED,
		FOREGROUND_GREEN | FOREGROUND_RED,
		FOREGROUND_GREEN,
		FOREGROUND_INTENSITY
	};
	SetConsoleTextAttribute(consoleHandle, arrLevelColor[level]);

	const LPDWORD outResult{};
	WriteConsoleA(consoleHandle,
	              szFormattedBuffer,
	              static_cast<DWORD>(strlen(szFormattedBuffer)),
	              outResult,
	              outResult);

	//OUTPUT-FILE
	char fileName[MAX_PATH]{};
	_snprintf_s(fileName,
	            std::size(fileName),
	            "LOG/LOG_%d-%d-%d-%d.txt",
	            systemtime.wYear,
	            systemtime.wMonth,
	            systemtime.wDay,
	            systemtime.wHour);

	FILE* pFile{};
	fopen_s(&pFile, fileName, "a");
	if (nullptr == pFile)
	{
		return;
	}
	fprintf_s(pFile, szFormattedBuffer);
	fflush(pFile);
	fclose(pFile);
}

void CLog::LogAssert(const char* pExpression, const char* pMessage, const char* pFile, int line)
{
	WriteLog(ERR,
	         "Asserion Failure :%s, message: %s, in file: %s, line: %d",
	         pExpression,
	         pMessage,
	         pFile,
	         line);
}

#endif
