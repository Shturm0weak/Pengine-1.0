#pragma once

#include "Core.h"

#include <map>
#include <unordered_map>
#include <vector>

namespace Pengine
{

	class PENGINE_API MemoryManager
	{
	private:

		struct MemoryPool
		{
			std::map<char*, uint64_t> m_MemoryPool;
			std::vector<char*> m_FreeMemory;
		};

		std::unordered_map<std::string, MemoryPool> m_MemoryPool;

		MemoryManager() = default;
		MemoryManager(const MemoryManager&) = delete;
		MemoryManager& operator=(const MemoryManager&) { return *this; }
		~MemoryManager() = default;
	public:

		static MemoryManager& GetInstance();

		void ShutDown();
		void FreeMemory(const std::string& type, class IAllocator* allocator);
		void AddPool(const std::string& type);
		MemoryPool* GetPool(const std::string& type);

		template<typename T>
		void FreeMemory(class IAllocator* allocator)
		{
			FreeMemory(Utils::GetTypeName<T>(), allocator);
		}

		template<typename T>
		MemoryPool* GetPool()
		{
			return GetPool(Utils::GetTypeName<T>());
		}

		template<typename T>
		T* Allocate()
		{
			std::string type = Utils::GetTypeName<T>();

			AddPool(type);
			MemoryPool* memoryPool = GetPool(type);

			char* ptr = Utils::PreAllocateMemory<T>(memoryPool->m_MemoryPool, memoryPool->m_FreeMemory);
			T* object = new ((void*)ptr) T();
			object->m_MemoryPoolPtr = ptr;
			object->m_IsInitialized = true;
			return object;
		}
	};

}
