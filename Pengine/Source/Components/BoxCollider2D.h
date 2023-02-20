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

		friend class CircleCollider2D;
	protected:

		virtual void Copy(const IComponent& component) override;

		virtual void Delete() override;

		virtual IComponent* New(GameObject* owner) override;
	public:

		b2PolygonShape m_PolygonShape;

		static IComponent* Create(GameObject* owner);

		BoxCollider2D() = default;

		~BoxCollider2D() = default;

		BoxCollider2D(const BoxCollider2D& bc2d);

		BoxCollider2D& operator=(const BoxCollider2D& bc2d);

		virtual ICollider2D* IntersectTrigger() override;

		void SetSize(const glm::vec2& size) { m_Size = size; }

		glm::vec2 GetSize() const { return m_Size; }

		bool BoxBoxOverlapSAT(BoxCollider2D* other);

		bool BoxCircleOverlap(class CircleCollider2D* other);
	};

}
