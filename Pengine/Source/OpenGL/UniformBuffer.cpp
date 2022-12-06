#include "UniformBuffer.h"

using namespace Pengine;

void UniformBuffer::Initialize(const void* data, uint32_t size, bool isStatic)
{
	m_Size = size;

	glGenBuffers(1, &m_RendererID);
	Bind();
	if (isStatic)
	{
		glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
	}
	else
	{
		glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
	}
}

void UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
	Bind();
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void UniformBuffer::Clear()
{
	m_Size = 0;
	m_RendererID = UINT32_MAX;
	glDeleteBuffers(1, &m_RendererID);
}

void UniformBuffer::BindBufferBase(uint32_t index)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, index, m_RendererID);
}

void UniformBuffer::BindBufferRange(uint32_t index, uint32_t start, uint32_t end)
{
	glBindBufferRange(GL_UNIFORM_BUFFER, index, m_RendererID, start, end);
}

UniformBuffer::~UniformBuffer()
{
	Clear();
}

void UniformBuffer::Bind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
}

void UniformBuffer::UnBind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}