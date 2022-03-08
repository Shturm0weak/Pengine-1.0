#pragma once

#include "../Core/Core.h"
 
#include <vector>
#include <glew.h>

namespace Pengine
{

	struct PENGINE_API VertexBufferElement
	{
		uint32_t m_Type;
		uint32_t m_Count;
		uint32_t m_Normalized;

		static uint32_t GetSizeOfType(uint32_t type)
		{
			switch (type)
			{
				case GL_FLOAT:			return 4;
				case GL_UNSIGNED_INT:	return 4;
				case GL_UNSIGNED_BYTE:	return 1;
				case GL_DOUBLE:			return 8;
			}
			return 0;
		}
	};

	class PENGINE_API VertexBufferLayout
	{
	private:

		std::vector<VertexBufferElement> m_Elements;
		uint32_t m_Stride;
	public:

		VertexBufferLayout() : m_Stride(0) {}

		const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
		uint32_t GetStride() const { return m_Stride; }

		void Clear() { 
			m_Stride = 0;
			m_Elements.clear(); 
		}

		template<typename T>
		void Push(uint32_t count)
		{
			static_assert(false);
		}

		template<>
		void Push<float>(uint32_t count)
		{
			m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
			m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
		}

		template<>
		void Push<double>(uint32_t count) 
		{
			m_Elements.push_back({ GL_DOUBLE, count, GL_FALSE });
			m_Stride += count * VertexBufferElement::GetSizeOfType(GL_DOUBLE);
		}

		template<>
		void Push<uint32_t>(uint32_t count)
		{
			m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
			m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
		}

		template<>
		void Push<unsigned char>(uint32_t count)
		{
			m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
			m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
		}
	};

}