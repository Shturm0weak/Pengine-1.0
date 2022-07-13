#pragma once

#include "Core.h"
#include "EntryPoint.h"

#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <future>
#include <map>

using Task = std::function<void()>;

namespace Pengine
{

	class PENGINE_API ThreadPool
	{
	private:

		std::vector<std::thread> m_Threads;
		std::map <std::thread::id, bool> m_IsThreadBusy;
		std::mutex m_Mutex;
		std::thread::id m_MainId = std::this_thread::get_id();
		std::condition_variable m_CondVar;
		std::queue <Task> m_Tasks;
		int m_ThreadsAmount = 0;
		bool m_IsStoped = false;

		void Initialize();

		ThreadPool() = default;
		ThreadPool(const ThreadPool&) = delete;
		ThreadPool& operator=(const ThreadPool&) { return *this; }
		~ThreadPool() = default;

		friend class EntryPoint;
	public:

		struct SyncParams
		{
		public:
			std::mutex s_Mtx;
			std::mutex s_ExtraMtx;
			std::atomic<bool>* s_Ready = nullptr;
			std::condition_variable s_CondVar;

			SyncParams();
			~SyncParams();

			void SetThreadFinished(size_t index);
			
			void WaitForAllThreads();
		};

		static ThreadPool& GetInstance();

		inline uint32_t GetThreadsAmount() { return m_Threads.size(); }
		
		void Shutdown();
		
		void Enqueue(Task task);
	};

}