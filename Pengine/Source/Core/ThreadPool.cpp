#include "ThreadPool.h"

using namespace Pengine;

void ThreadPool::Initialize()
{
	m_ThreadsAmount = std::thread::hardware_concurrency() - 1;
	for (unsigned int i = 0u; i < m_ThreadsAmount; ++i)
	{
		m_Threads.emplace_back([=] {
			while (true)
			{
				if (m_MainId == std::this_thread::get_id())
					return;
				auto iter = m_IsThreadBusy.find(std::this_thread::get_id());

				Task task;
				{
					std::unique_lock <std::mutex> umutex(m_Mutex);
					m_CondVar.wait(umutex, [=] { return m_IsStoped || !m_Tasks.empty(); });
					if (m_Tasks.empty() && m_IsStoped) break;

					task = std::move(m_Tasks.front());
					m_Tasks.pop();

					if (iter != m_IsThreadBusy.end())
						iter->second = true;
					//std::cout << std::this_thread::get_id() << std::endl;
				}
				task();
				if (iter != m_IsThreadBusy.end())
					iter->second = false;
			}
			});
		m_IsThreadBusy.insert(std::make_pair(m_Threads.back().get_id(), false));
	}
	Logger::Success("ThreadPool has been initialized!");
}

ThreadPool& ThreadPool::GetInstance()
{
	static ThreadPool threadPool;
	return threadPool;
}

void ThreadPool::Shutdown()
{
	{
		std::unique_lock <std::mutex> umutex(m_Mutex);
		m_IsStoped = true;
	}
	m_CondVar.notify_all();
	for (std::thread& thread : m_Threads)
	{
		thread.detach();
	}
}

void ThreadPool::Enqueue(Task task)
{
	{
		std::unique_lock<std::mutex> lock{ m_Mutex };
		m_Tasks.emplace(std::move(task));
	}
	m_CondVar.notify_one();
}

ThreadPool::SyncParams::SyncParams()
{
	size_t numThreads = ThreadPool::GetInstance().GetThreadsAmount();
	s_Ready = new std::atomic<bool>[numThreads];
	for (size_t i = 0; i < numThreads; i++)
	{
		s_Ready[i] = true;
	}
}

ThreadPool::SyncParams::~SyncParams()
{
	delete[] s_Ready;
}

void ThreadPool::SyncParams::SetThreadFinished(size_t index)
{
	std::lock_guard lock(s_Mtx);
	s_Ready[index] = true;
	s_CondVar.notify_all();
}

void ThreadPool::SyncParams::WaitForAllThreads()
{
	size_t numThreads = ThreadPool::GetInstance().GetThreadsAmount();
	std::unique_lock<std::mutex> lock(s_Mtx);
	s_CondVar.wait(lock, [=] {
		for (size_t i = 0; i < numThreads; i++)
		{
			if (s_Ready[i] == false)
			{
				return false;
			}
		}
		return true;
	});
}
