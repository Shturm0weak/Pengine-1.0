#pragma once

#include "../Core/Core.h"

namespace Pengine
{

	class PENGINE_API VertexBuffer
	{
	private:

		uint32_t m_RendererId = UINT32_MAX;
	public:

		VertexBuffer() = default;
		~VertexBuffer();

		uint32_t GetRendererId() const { return m_RendererId; }
		void Initialize(const void* data, uint32_t size, bool Static = true);
		void Clear();
		void Bind() const;
		void UnBind() const;
		void Invalidate() const;
	};

}