#pragma once

#include "Core.h"

namespace Pengine
{

	struct PENGINE_API BoundingBox
	{
		glm::vec3 m_Min = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Max = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Center = { 0.0f, 0.0f, 0.0f };

		void GetTransformedBoundaries(const glm::mat4& position, const glm::mat4& rotation, const glm::mat4& scale,
			glm::vec3& min, glm::vec3& max);

		glm::vec3 GetTransformedCenter(const glm::vec3& position);
	};

}
