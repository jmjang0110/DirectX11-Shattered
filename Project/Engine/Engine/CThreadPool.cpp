#include "pch.h"
#include "CThreadPool.h"

CThreadPool::CThreadPool()
	: m_threadCount(std::thread::hardware_concurrency())
{
	m_vecThread.reserve(m_threadCount);
	for (size_t i = 0; i < m_threadCount; ++i)
	{
		m_vecThread.emplace_back([this]()
		{
			this->WorkerThread();
			m_condition.notify_one();
		});
	}
}

void CThreadPool::Join()
{
	std::unique_lock<std::mutex> lock{m_lock};
	m_allFinished.wait(lock, [this]() { return this->m_vecWork.empty() && m_busy == 0; });
}

void CThreadPool::WorkerThread()
{
	while (true)
	{
		std::unique_lock<std::mutex> guard{m_lock};
		m_condition.wait(guard, [this]() { return false == this->m_vecWork.empty() || m_stop; });

		if (m_vecWork.empty())
		{
			return;
		}

		std::function<void()> work = std::move(m_vecWork.front());
		m_vecWork.pop();
		guard.unlock();

		++m_busy;
		work();
		--m_busy;
		m_allFinished.notify_one();
	}
}

CThreadPool::~CThreadPool()
{
	m_stop = true;
	m_condition.notify_all();

	for (auto& jobThread : m_vecThread)
	{
		jobThread.join();
	}
}
