#pragma once

#include "../Core/Core.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace Pengine
{

	class PENGINE_API VertexArray
	{
	private:

		uint32_t m_RendererId = UINT32_MAX;
	public:

		VertexArray() = default;
		~VertexArray();

		uint32_t GetRendererId() const { return m_RendererId; }
		void Initialize();
		void Clear();
		void Bind() const;
		void UnBind() const;
		void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, 
			const GLuint offsetOfGlVertexAttribArray = 0,const GLuint devisor = 0);
	};

}