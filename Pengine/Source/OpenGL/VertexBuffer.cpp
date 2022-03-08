#include "VertexBuffer.h"

#include <glew.h>
#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

using namespace Pengine;

void VertexBuffer::Initialize(const void* data, uint32_t size, bool Static)
{
	glGenBuffers(1, &m_RendererId);
	Bind();
	if (Static)
	{
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}
	else
	{
		glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
	}
}

void VertexBuffer::Clear()
{
	m_RendererId = UINT32_MAX;
	glDeleteBuffers(1, &m_RendererId);
}

VertexBuffer::~VertexBuffer()
{
	Clear();
}

void VertexBuffer::Bind() const 
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
}

void VertexBuffer::UnBind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::Invalidate() const
{
	glInvalidateBufferData(m_RendererId);
}