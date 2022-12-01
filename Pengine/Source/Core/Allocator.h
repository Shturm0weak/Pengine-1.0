#pragma once

#include "Core.h"

namespace Pengine
{

	class PENGINE_API IAllocator
	{
	public:

		bool IsInitialized() const { return m_IsInitialized; }
	private:

		char* m_MemoryPoolPtr = nullptr;
		bool m_IsInitialized = false;

		friend class MemoryManager;
	};

}
