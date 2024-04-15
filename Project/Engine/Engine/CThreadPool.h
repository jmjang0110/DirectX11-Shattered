#pragma once
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include "singleton.h"

class CThreadPool : public CSingleton<CThreadPool>
{
public:
	CThreadPool();
	virtual ~CThreadPool();

public:
	template <typename F, typename... Args>
	std::future<std::invoke_result_t<F, Args ...>> EnqueueJob(F&& f, Args&&...args);

public:
	void Start()
	{
		m_stop = false;
		m_condition.notify_all();
	}

	void Join();
private:
	void WorkerThread();
private:
	// 총 Worker 쓰레드의 개수.
	size_t                            m_threadCount;
	std::vector<std::thread>          m_vecThread;
	std::queue<std::function<void()>> m_vecWork;
	std::condition_variable           m_condition;
	std::mutex                        m_lock;

	std::condition_variable m_allFinished;
	std::atomic<int>        m_busy = 0;
	std::atomic<bool>       m_stop = false;
};

template <typename F, typename... Args>
std::future<std::invoke_result_t<F, Args ...>> CThreadPool::EnqueueJob(F&& f, Args&&...args)
{
	using return_t = std::invoke_result_t<F, Args ...>;
	auto pWork = std::make_shared<std::packaged_task<return_t()>>(std::bind(std::forward<F>(f),
	                                                                        std::forward<Args>(args)...));
	std::future<return_t> resultFuture = pWork->get_future();

	{
		std::lock_guard<std::mutex> lock(m_lock);
		m_vecWork.push([pWork]() { (*pWork)(); });
	}


	return resultFuture;
}
