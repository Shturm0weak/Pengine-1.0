#pragma once

#include "../Core/Core.h"
#include "ICollider2D.h"
#include "Box2D/include/box2d/b2_fixture.h"
#include "Box2D/include/box2d/b2_polygon_shape.h"

namespace Pengine
{

	class PENGINE_API BoxCollider2D : public ICollider2D
	{
	private:

		glm::vec2 m_Size = { 0.5f, 0.5f };

		bool BoxBoxOverlapSAT(BoxCollider2D* other);
		
		bool BoxCircleOverlap(class CircleCollider2D* other);

		friend class CircleCollider2D;
	public:

		b2PolygonShape m_PolygonShape;

		static IComponent* Create(GameObject* owner);

		BoxCollider2D() = default;

		virtual void Copy(const IComponent& component) override;
		
		virtual void Delete() override;
		
		virtual IComponent* New(GameObject* owner) override;
		
		virtual ICollider2D* IntersectTrigger() override;

		virtual void SetSize(const glm::vec2& size) override { m_Size = size; }

		glm::vec2 GetSize() const { return m_Size; }
		
		glm::vec2 GetOffset() const { return m_Offset; }
		
		void SetOffset(const glm::vec2& offset) { m_Offset = offset; }
		
		glm::vec2 GetPosition() const;
	};

}
