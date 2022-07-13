#pragma once

#include "Core.h"
#include "../Components/ICollider2D.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API Raycast2D
	{
	public:

		class PENGINE_API Hit2D
		{
		private:

			class ICollider2D* m_Collider = nullptr;
			glm::vec2 m_Position;

			friend class Raycast2D;
		public:

			class ICollider2D* GetCollider() const { return m_Collider; }
			
			glm::vec2 GetPosition() const { return m_Position; }
		};
		
		static bool LineIntersect(const glm::vec2& from, const glm::vec2& to, const glm::vec2& start, const glm::vec2& end, glm::vec2& where);
		
		static bool CircleIntersect(const glm::vec2& from, const glm::vec2& to, const glm::vec2& circlePosition, float radius, glm::vec2& where);

		static void Raycast(class Scene* scene, const glm::vec2& from, const glm::vec2& to, Hit2D& hit,
			std::vector<std::string>& ignoreMaskByTag = std::vector<std::string>(),
			std::vector<ICollider2D*>& ignoreMaskByCollider = std::vector<ICollider2D*>());
	};

}