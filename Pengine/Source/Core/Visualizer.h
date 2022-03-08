#pragma once

#include "Core.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API Visualizer
	{
	private:

		struct LineParams
		{
			glm::vec3 m_Start;
			glm::vec3 m_End;
			glm::vec4 m_Color;
		};

		struct QuadParams
		{
			glm::mat4 m_Transform;
			glm::vec4 m_Color;
			class Texture* m_Texture;
		};

		static std::vector<LineParams> m_Lines;
		static std::vector<QuadParams> m_Quads;

		static void RenderLines();
		static void RenderQuads();

		friend class EntryPoint;
	public:

		static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f), class Texture* texture = nullptr);
	};

}
