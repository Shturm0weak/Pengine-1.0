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
			std::mutex m_Mtx;
			std::mutex m_ExtraMtx;
			std::atomic<bool>* m_Ready = nullptr;
			std::condition_variable m_CondVar;
			size_t m_Threads = 0;

			SyncParams(size_t threads);
			~SyncParams();

			void SetThreadFinished(size_t index, bool isFinished = true);
			
			void WaitForAllThreads();
		};

		static ThreadPool& GetInstance();

		inline uint32_t GetThreadsAmount() { return m_Threads.size(); }
		
		void Shutdown();
		
		void Enqueue(Task task);

		bool IsMainThread() const { return m_MainId == std::this_thread::get_id(); }
	};

}