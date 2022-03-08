#include "MemoryManager.h"

#include "Allocator.h"
#include "Utils.h"

using namespace Pengine;

MemoryManager& MemoryManager::GetInstance()
{
    static MemoryManager memoryManager;
    return memoryManager;
}

void MemoryManager::ShutDown()
{
    for (auto memoryPool : m_MemoryPool)
    {
        for (auto memoryPtr : memoryPool.second.m_MemoryPool)
        {
            delete[] memoryPtr.first;
        }
    }
}

void MemoryManager::FreeMemory(const std::string& type, IAllocator* allocator)
{
    assert(allocator);

    MemoryPool* memoryPool = GetPool(type);
    if (memoryPool)
    {
        memoryPool->m_FreeMemory.push_back(allocator->m_MemoryPoolPtr);
        memoryPool->m_MemoryPool.rbegin()->second--;
        allocator->m_IsInitialized = false;
    }
}

void MemoryManager::AddPool(const std::string& type)
{
    m_MemoryPool.insert(std::make_pair(type, MemoryPool()));
}

MemoryManager::MemoryPool* MemoryManager::GetPool(const std::string& type)
{
    auto memoryPoolIter = m_MemoryPool.find(type);
    if (memoryPoolIter != m_MemoryPool.end())
    {
        return &memoryPoolIter->second;
    }
    return nullptr;
}
