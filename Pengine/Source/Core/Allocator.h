#pragma once

#include "Core.h"

namespace Pengine
{

	class PENGINE_API IAllocator
	{
	public:

		bool m_IsInitialized = false;
	private:

		char* m_MemoryPoolPtr = nullptr;

		friend class MemoryManager;
	};

}
