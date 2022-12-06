#pragma once

#include "../Core/Core.h"

namespace Pengine
{

	class PENGINE_API UniformBuffer
	{
	private:

		uint32_t m_RendererID = UINT32_MAX;
		uint32_t m_Size = 0;
	public:

		UniformBuffer() = default;
		~UniformBuffer();

		uint32_t GetRendererID() const { return m_RendererID; }

		uint32_t GetSize() const { return m_Size; }

		void Initialize(const void* data, uint32_t size, bool isStatic);

		void SetData(const void* data, uint32_t size, uint32_t offset);

		void Clear();

		void BindBufferBase(uint32_t index);

		void BindBufferRange(uint32_t index, uint32_t start, uint32_t end);

		void Bind() const;

		void UnBind() const;

	};

}

