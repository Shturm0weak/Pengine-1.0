#include "Visualizer.h"

#include "MeshManager.h"
#include "Utils.h"
#include "Window.h"
#include "../OpenGL/Batch.h"

using namespace Pengine;

void Visualizer::RenderLines()
{
	Batch::GetInstance().BeginLines();

	std::vector<LineParams> linesForTheNextFrame;
	linesForTheNextFrame.reserve(1000);

	size_t size = m_Lines.size();
	for (size_t i = 0; i < size; i++)
	{
		LineParams& line = m_Lines[i];
		Batch::GetInstance().Submit(line.m_Start, line.m_End, line.m_Color);

		line.duration -= Time::GetDeltaTime();
		if (line.duration > 0.0f)
		{
			linesForTheNextFrame.emplace_back(line);
		}
	}
	m_Lines.clear();
	m_Lines = std::move(linesForTheNextFrame);

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

void Visualizer::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float duration)
{
	m_Lines.push_back({ start, end, color, duration });
}

void Visualizer::DrawQuad(const glm::mat4& transform, const glm::vec4& color, Texture* texture)
{
	m_Quads.push_back({ transform, color, texture });
}

void Visualizer::DrawWireFrameCube(const glm::mat4& position, const glm::mat4& rotation, const glm::mat4& scale, 
	const glm::vec3& min, const glm::vec3& max, const glm::vec4& color)
{
	glm::vec3 scaleVector = Utils::GetScale(scale);

	// AntiClockWise, First digit is 0 - FrontFacePoint, 1 - BackFacePoint.
	glm::vec3 point00 = position * rotation * (glm::vec4(min.x, min.y, min.z, 1.0f) * scale);
	glm::vec3 point01 = position * rotation * (glm::vec4(max.x, min.y, min.z, 1.0f) * scale);
	glm::vec3 point02 = position * rotation * (glm::vec4(max.x, max.y, min.z, 1.0f) * scale);
	glm::vec3 point03 = position * rotation * (glm::vec4(min.x, max.y, min.z, 1.0f) * scale);

	glm::vec3 point10 = position * rotation * (glm::vec4(min.x, min.y, max.z, 1.0f) * scale);
	glm::vec3 point11 = position * rotation * (glm::vec4(max.x, min.y, max.z, 1.0f) * scale);
	glm::vec3 point12 = position * rotation * (glm::vec4(max.x, max.y, max.z, 1.0f) * scale);
	glm::vec3 point13 = position * rotation * (glm::vec4(min.x, max.y, max.z, 1.0f) * scale);

	// Front.
	DrawLine(point00, point01, color);
	DrawLine(point01, point02, color);
	DrawLine(point02, point03, color);
	DrawLine(point03, point00, color);

	// Back.
	DrawLine(point10, point11, color);
	DrawLine(point11, point12, color);
	DrawLine(point12, point13, color);
	DrawLine(point13, point10, color);

	// Left.
	DrawLine(point00, point10, color);
	DrawLine(point03, point13, color);

	// Right.
	DrawLine(point01, point11, color);
	DrawLine(point02, point12, color);
}

void Visualizer::DrawCircle(float innerRadius, float outerRadius, const glm::mat4& transform, const glm::vec4& color, Texture* texture)
{
	m_Circles.push_back({ transform, color, texture, innerRadius, outerRadius });
}

void Visualizer::DrawWireFrameSphere(int rows, int cols, float innerRadius, float outerRadius, const glm::mat4& position,
	const glm::mat4& rotation,  const glm::mat4& scale, const glm::vec4& color, Texture* texture)
{
	const float rowAngleStep = glm::radians(180.0f) / rows;
	for (size_t i = 0; i < rows; i++)
	{
		glm::mat4 transformRotated = position * rotation * glm::toMat4(glm::quat({ i * rowAngleStep, 0.0f, 0.0f })) * scale;
		m_Circles.push_back({ transformRotated, color, texture, innerRadius, outerRadius });
	}

	const float colAngleStep = glm::radians(180.0f) / cols;
	for (size_t i = 0; i < cols; i++)
	{
		glm::mat4 transformRotated = position * rotation * glm::toMat4(glm::quat({ 0.0f, glm::radians(90.0f), i * colAngleStep })) * scale;
		m_Circles.push_back({ transformRotated, color, texture, innerRadius, outerRadius });
	}
}
