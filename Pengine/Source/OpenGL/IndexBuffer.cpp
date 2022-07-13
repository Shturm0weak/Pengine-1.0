#include "IndexBuffer.h"

#include <glew.h>
#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

using namespace Pengine;

void IndexBuffer::Initialize(const uint32_t* data, uint32_t count)
{
	m_Count = count;
	glGenBuffers(1, &m_RendererID);
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_DYNAMIC_DRAW);
}

void IndexBuffer::Clear()
{
	m_RendererID = UINT32_MAX;
	m_Count = 0;
	glDeleteBuffers(1, &m_RendererID);
}

IndexBuffer::~IndexBuffer()
{
	Clear();
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void IndexBuffer::UnBind() const 
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}