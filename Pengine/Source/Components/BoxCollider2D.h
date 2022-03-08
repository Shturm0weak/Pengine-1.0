#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"
#include "../Core/Allocator.h"
#include "Box2D/include/box2d/b2_fixture.h"
#include "Box2D/include/box2d/b2_polygon_shape.h"

namespace Pengine
{

	class PENGINE_API BoxCollider2D : public IComponent, public IAllocator
	{
	public:

		b2PolygonShape m_PolygonShape;
		b2FixtureDef m_Fixture;

		glm::vec2 m_Offset;
		glm::vec2 m_Size = { 0.5f, 0.5f };

		float m_Density = 1.0f;
		float m_Friction = 0.5f;
		float m_Restitution = 0.0f;
		float m_RestitutionThreshold = 0.5f;

		static IComponent* Create(GameObject* owner);

		BoxCollider2D() = default;

		virtual void Copy(const IComponent& component) override;
		virtual IComponent* CreateCopy(GameObject* newOwner) override;
		virtual void Delete() override;
	
		void operator=(const BoxCollider2D& bc2d);
	};

}
