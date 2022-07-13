#pragma once

#include "../Core/Core.h"

namespace Pengine
{

	class PENGINE_API IndexBuffer
	{
	private:

		uint32_t m_RendererID = UINT32_MAX;
		uint32_t m_Count = 0;
	public:

		IndexBuffer() = default;
		~IndexBuffer();

		uint32_t GetCount() const { return m_Count; }
		
		uint32_t GetRendererID() const { return m_RendererID; }
		
		void Initialize(const uint32_t* data, uint32_t count);
		
		void Clear();
		
		void Bind() const;
		
		void UnBind() const;
	};

}