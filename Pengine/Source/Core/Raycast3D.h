#pragma once

#include "Core.h"
#include "BoundingBox.h"
#include "../Components/Transform.h"

#include <map>
#include <vector>

namespace Pengine
{

	class PENGINE_API Raycast3D 
	{
	public:

		struct Hit3D
		{
			glm::vec3 m_Point;
			class GameObject* m_Object = nullptr;
			float m_Distance = 0.0f;
		};

		static std::map<float, class GameObject*> RayCast(std::vector<class GameObject*> gameObjects, const glm::vec3& start, const glm::vec3& direction, Hit3D& hit, float length,
			bool AABB = false, std::vector<std::string> ignoreMask = std::vector<std::string>());

		static std::map<float, class BoxCollider3D*> RayCast(std::vector<class BoxCollider3D*> bcs3d, const glm::vec3& start, const glm::vec3& direction, Hit3D& hit, float length,
			bool AABB = false, std::vector<std::string> ignoreMask = std::vector<std::string>());

		static bool IntersectTriangle(const const glm::vec3& start, const glm::vec3& direction, Hit3D& hit, float length, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& planeNorm);
		static bool IntersectBoxAABB(const glm::vec3& start, const glm::vec3& direction, Hit3D& hit, float length, const BoundingBox& boundingBox, const Transform& transform);
		static bool IntersectBoxOBB(const glm::vec3& start, const glm::vec3& direction, Hit3D& hit, float length, const BoundingBox& boundingBox, const Transform& transform);
	};

}