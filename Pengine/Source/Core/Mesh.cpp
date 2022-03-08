#include "Mesh.h"

using namespace Pengine;

Mesh::Mesh(const std::string& name, std::vector<float>& vertexAttributes, std::vector<uint32_t>& indices,
	const std::vector<uint32_t>& layouts, const std::string& filePath)
	: m_VertexAttributes(std::move(vertexAttributes))
	, m_Indices(std::move(indices))
	, m_Name(name)
	, m_FilePath(filePath)
{
	m_Va.Initialize();
	m_Vb.Initialize(&m_VertexAttributes[0], m_VertexAttributes.size() * sizeof(float));
	for (auto layout : layouts)
	{
		m_Layout.Push<float>(layout);
	}
	m_Va.AddBuffer(m_Vb, m_Layout);
	m_Ib.Initialize(&m_Indices[0], m_Indices.size());
}

void Mesh::Bind() const
{
	m_Ib.Bind();
	m_Vb.Bind();
	m_Va.Bind();
}

void Mesh::UnBind() const
{
	m_Ib.UnBind();
	m_Vb.UnBind();
	m_Va.UnBind();
}
