#pragma once

class CTimeMeasure
{
private:
	const char* m_pName;
	bool        m_bStopped;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_startPoint;

public:
	void Stop() const
	{
		const auto endPoint = std::chrono::high_resolution_clock::now();
		const auto diff     = endPoint - m_startPoint;

		const auto  duration     = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
		const float timeInSecond = duration * 0.001f;
		LOG_TRACE("TIME CONSUMED : %s, %.3fms", m_pName, timeInSecond);
	}

public:
	explicit CTimeMeasure(const char* pName)
		: m_pName{pName}
		, m_bStopped{false}
		, m_startPoint{std::chrono::high_resolution_clock::now()} { }

	~CTimeMeasure()
	{
		if (false == m_bStopped)
		{
			Stop();
		}
	}
};

#ifdef _DEBUG
#define TIME_MEASURE(name) CTimeMeasure timeMeasure##__LINE__{name};
#else
#define TIME_MEASURE(name)
#endif
