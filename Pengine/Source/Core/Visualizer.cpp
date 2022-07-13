#include "Visualizer.h"

#include "MeshManager.h"
#include "../OpenGL/Batch.h"

using namespace Pengine;

void Visualizer::RenderLines()
{
	Batch::GetInstance().BeginLines();

	size_t size = m_Lines.size();
	for (size_t i = 0; i < size; i++)
	{
		Batch::GetInstance().Submit(m_Lines[i].m_Start, m_Lines[i].m_End, m_Lines[i].m_Color);
	}
	m_Lines.clear();

	Batch::GetInstance().EndLines();
}

void Visualizer::RenderQuads()
{
	size_t size = m_Quads.size();
	const std::vector<float>& meshVertexAttributes = MeshManager::GetInstance().Get("Quad")->GetVertexAttributes();
	for (size_t i = 0; i < size; i++)
	{
		Batch::GetInstance().Submit(meshVertexAttributes, m_Quads[i].m_Transform, m_Quads[i].m_Color, { m_Quads[i].m_Texture });
	}
	m_Quads.clear();
}

void Visualizer::RenderCircles()
{
	size_t size = m_Circles.size();
	const std::vector<float>& meshVertexAttributes = MeshManager::GetInstance().Get("Quad")->GetVertexAttributes();
	for (size_t i = 0; i < size; i++)
	{
		Batch::GetInstance().Submit(meshVertexAttributes, m_Circles[i].m_Transform, m_Circles[i].m_Color, { m_Circles[i].m_Texture }, { m_Circles[i].m_InnerRadius, m_Circles[i].m_OuterRadius, 0.0f, 1.0f });
	}
	m_Circles.clear();
}

void Visualizer::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
{
	m_Lines.push_back({ start, end, color });
}

void Visualizer::DrawQuad(const glm::mat4& transform, const glm::vec4& color, Texture* texture)
{
	m_Quads.push_back({ transform, color, texture });
}

void Visualizer::DrawCircle(float innerRadius, float outerRadius, const glm::mat4& transform, const glm::vec4& color, Texture* texture)
{
	m_Circles.push_back({ transform, color, texture, innerRadius, outerRadius });
}
