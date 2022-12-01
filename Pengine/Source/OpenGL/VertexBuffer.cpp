#include "VertexBuffer.h"

#include <glew.h>
#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

using namespace Pengine;

void VertexBuffer::Initialize(const void* data, uint32_t size, bool isStatic)
{
	glGenBuffers(1, &m_RendererID);
	Bind();
	if (isStatic)
	{
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}
	else
	{
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	}
}

void VertexBuffer::Clear()
{
	m_RendererID = UINT32_MAX;
	glDeleteBuffers(1, &m_RendererID);
}

VertexBuffer::~VertexBuffer()
{
	Clear();
}

void VertexBuffer::Bind() const 
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::UnBind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::Invalidate() const
{
	glDeleteBuffers(1, &m_RendererID);
}