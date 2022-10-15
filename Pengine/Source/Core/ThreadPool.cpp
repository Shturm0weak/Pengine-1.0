#include "ThreadPool.h"

using namespace Pengine;

void ThreadPool::Initialize()
{
	m_ThreadsAmount = std::thread::hardware_concurrency() - 1;

	for (int i = 0u; i < m_ThreadsAmount; i++)
	{
		m_Threads.emplace_back([=] {
			while (true)
			{
				if (m_MainId == std::this_thread::get_id())
				{
					return;
				}

				auto isThreadBusy = m_IsThreadBusy.find(std::this_thread::get_id());

				Task task;
				{
					std::unique_lock <std::mutex> umutex(m_Mutex);
					m_CondVar.wait(umutex, 
						[=] 
					{ 
						return m_IsStoped || !m_Tasks.empty(); 
					});
					
					if (m_Tasks.empty() && m_IsStoped)
					{
						break;
					}

					task = std::move(m_Tasks.front());
					m_Tasks.pop();

					if (isThreadBusy != m_IsThreadBusy.end())
					{
						isThreadBusy->second = true;
					}
					//std::cout << std::this_thread::get_id() << std::endl;
				}
				task();

				if (isThreadBusy != m_IsThreadBusy.end())
				{
					isThreadBusy->second = false;
				}
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

ThreadPool::SyncParams::SyncParams(size_t threads)
	: m_Threads(threads)
{
	m_Ready = new std::atomic<bool>[m_Threads];
	for (size_t i = 0; i < m_Threads; i++)
	{
		m_Ready[i] = true;
	}
}

ThreadPool::SyncParams::~SyncParams()
{
	delete[] m_Ready;
}

void ThreadPool::SyncParams::SetThreadFinished(size_t index, bool isFinished)
{
	std::lock_guard lock(m_Mtx);
	m_Ready[index] = isFinished;

	if (isFinished)
	{
		m_CondVar.notify_all();
	}
}

void ThreadPool::SyncParams::WaitForAllThreads()
{
	std::unique_lock<std::mutex> lock(m_Mtx);
	m_CondVar.wait(lock, 
		[=]
	{
		for (size_t i = 0; i < m_Threads; i++)
		{
			if (m_Ready[i] == false)
			{
				return false;
			}
		}

		return true;
	});
}
