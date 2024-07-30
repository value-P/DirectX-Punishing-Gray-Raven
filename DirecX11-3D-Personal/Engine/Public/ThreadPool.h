#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CThreadPool
	{
	public:
		CThreadPool(_uint iNumThreads);
		virtual ~CThreadPool();

	public:

	private:
		void WorkerThread();

	private:
		_uint m_iNumThread = 0;
		
		vector<thread> worker_Threads;
		queue<function<void()>> jobs;

		condition_variable conditionVariable_jobQ = {};
		mutex mutex_jobQ = {};

		bool stop_all = false;
	};
}


