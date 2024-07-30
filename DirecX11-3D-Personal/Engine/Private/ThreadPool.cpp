#include "ThreadPool.h"

Engine::CThreadPool::CThreadPool(_uint iNumThreads)
	:m_iNumThread(iNumThreads)
{
	worker_Threads.reserve(iNumThreads);
	for (_uint i = 0; i < iNumThreads; i++)
		worker_Threads.emplace_back([this]() {this->WorkerThread(); });
}

Engine::CThreadPool::~CThreadPool()
{
	stop_all = true;
	conditionVariable_jobQ.notify_all();

	for (auto& thread : worker_Threads)
		thread.join();
}

void Engine::CThreadPool::WorkerThread()
{
	while (true)
	{
		unique_lock<mutex> lock(mutex_jobQ);

		conditionVariable_jobQ.wait(lock, [this]() {return !this->jobs.empty() || stop_all; });

		if (stop_all && this->jobs.empty())
			return;

		// 맨 앞의 job을 뺀다
		function<void()> job = move(jobs.front());
		jobs.pop();

		lock.unlock();

		job();
	}
}
