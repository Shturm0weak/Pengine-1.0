#include "BoundingBox.h"

using namespace Pengine;

void BoundingBox::GetTransformed(const glm::mat4& position, const glm::mat4& rotation, const glm::mat4& scale, glm::vec3& min, glm::vec3& max)
{
	min = position * rotation * (glm::vec4(m_Min.x, m_Min.y, m_Min.z, 1.0f) * scale);
	max = position * rotation * (glm::vec4(m_Max.x, m_Max.y, m_Max.z, 1.0f) * scale);
}