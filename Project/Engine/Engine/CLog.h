#pragma once

#define USE_THIRD_PARTY_LOGGER 0

#if USE_THIRD_PARTY_LOGGER
#include <spdlog/spdlog.h>
#ifdef _DEBUG
#pragma comment(lib,"spdlog/spdlogd")
#else
#pragma comment(lib,"spdlog/spdlog")
#endif


#include <memory>

class CLog final
{
private:
	inline static std::shared_ptr<spdlog::logger> s_logger{};

public:
	static void Init();

public:
	static std::shared_ptr<spdlog::logger>& GetLogger() { return s_logger; }
};

#ifdef _DEBUG
#define LOG_ERROR(...) CLog::GetLogger()->error(__VA_ARGS__)
#define LOG_WARN(...)  CLog::GetLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...)  CLog::GetLogger()->info(__VA_ARGS__)
#define LOG_TRACE(...) CLog::GetLogger()->trace(__VA_ARGS__)
#define LOG_ASSERT(expr,...)																		\
	if(!(expr))																						\
	{																								\
		CLog::GetLogger()->error("Asserion Failure :{0}, message: {1}, in file: {2}, line: {3}",	\
								#expr,																\
								__VA_ARGS__,														\
								__FILE__,															\
								__LINE__);															\
		assert(expr);																				\
	}

#else
#define LOG_ERROR(...)
#define LOG_WARN(...) 
#define LOG_INFO(...) 
#define LOG_TRACE(...)
#define LOG_ASSERT(expr,...)
#endif

#else


class CLog final
{
public:
	enum LOG_LEVEL
	{
		ERR,
		WARN,
		INFO,
		TRACE,
		END
	};

public:
	static void Init();
	static void WriteLog(LOG_LEVEL level, const char* pMessage, ...);
	static void LogAssert(const char* pExpression, const char* pMessage, const char* pFile, int line);

private:
	inline static std::mutex s_mutex{};
};

#ifdef _DEBUG
#define LOG_ERROR(message,...) CLog::WriteLog(CLog::LOG_LEVEL::ERR,message,__VA_ARGS__)
#define LOG_WARN(message,...)  CLog::WriteLog(CLog::LOG_LEVEL::WARN,message,__VA_ARGS__)
#define LOG_INFO(message,...)  CLog::WriteLog(CLog::LOG_LEVEL::INFO,message,__VA_ARGS__)
#define LOG_TRACE(message,...) CLog::WriteLog(CLog::LOG_LEVEL::TRACE,message,__VA_ARGS__)
#define LOG_ASSERT(expr,...)									\
	if(!(expr))													\
	{															\
		CLog::LogAssert(#expr,__VA_ARGS__,__FILE__,__LINE__);	\
		assert(expr);											\
	}
#else
#define LOG_ERROR(message,...)
#define LOG_WARN(message,...) 
#define LOG_INFO(message,...) 
#define LOG_TRACE(message,...)
#define LOG_ASSERT(expr,...)
#endif

#endif
