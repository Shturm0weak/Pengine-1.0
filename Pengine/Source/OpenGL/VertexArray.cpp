#include "VertexArray.h"

using namespace Pengine;

void VertexArray::Initialize()
{
	glGenVertexArrays(1, &m_RendererId);
	Bind();
}

void VertexArray::Clear()
{
	m_RendererId = UINT32_MAX;
	glDeleteVertexArrays(1, &m_RendererId);
}

VertexArray::~VertexArray()
{
	Clear();
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, 
	const GLuint offsetOfGlVertexAttribArray, const GLuint devisor) 
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	uint32_t offset = 0;
	for(uint32_t i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glEnableVertexAttribArray(i + offsetOfGlVertexAttribArray);
		glVertexAttribPointer(i + offsetOfGlVertexAttribArray, element.m_Count, element.m_Type,
			element.m_Normalized, layout.GetStride(), (const void*)offset);
		glVertexAttribDivisor(i + offsetOfGlVertexAttribArray, devisor);
		offset += element.m_Count * VertexBufferElement::GetSizeOfType(element.m_Type);
	}
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_RendererId);
}

void VertexArray::UnBind() const
{
	glBindVertexArray(0);
}

