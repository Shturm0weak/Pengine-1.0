#pragma once

#include "../Core/Core.h"
#include "ICollider2D.h"
#include "Box2D/include/box2d/b2_fixture.h"
#include "Box2D/include/box2d/b2_circle_shape.h"

namespace Pengine
{

	class PENGINE_API CircleCollider2D : public ICollider2D
	{
	private:

		float m_Radius = 0.5f;
	protected:

		virtual void Copy(const IComponent& component) override;

		virtual void Move(IComponent&& component) override;

		virtual void Delete() override;

		virtual IComponent* New(GameObject* owner) override;
	public:

		b2CircleShape m_CircleShape;

		static IComponent* Create(GameObject* owner);

		CircleCollider2D() = default;

		~CircleCollider2D() = default;

		CircleCollider2D(const CircleCollider2D & bc2d);

		CircleCollider2D(CircleCollider2D && bc2d) noexcept;

		CircleCollider2D& operator=(const CircleCollider2D& bc2d);

		CircleCollider2D& operator=(CircleCollider2D&& bc2d) noexcept;

		virtual ICollider2D* IntersectTrigger() override;

		void SetRadius(float radius) { m_Radius = radius; }
		
		float GetRadius() const { return m_Radius; }
	
		bool CircleCircleOverlap(CircleCollider2D* other);
	};

}
