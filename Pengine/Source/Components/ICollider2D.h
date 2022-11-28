#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"
#include "../Core/Allocator.h"
#include "Box2D/include/box2d/b2_fixture.h"
#include "Box2D/include/box2d/b2_polygon_shape.h"

namespace Pengine
{

	class PENGINE_API ICollider2D : public IComponent, public IAllocator
	{
	public:

		b2Shape* m_Shape;
		b2FixtureDef m_Fixture;

		std::string m_Tag;

		glm::vec2 m_Offset = { 0.0f, 0.0f };

		float m_Density = 1.0f;
		float m_Friction = 0.5f;
		float m_Restitution = 0.0f;
		float m_RestitutionThreshold = 0.5f;
		bool m_IsTrigger = false;

		ICollider2D() { m_Type = "ICollider2D"; }
		
		void SetTrigger(bool isTrigger) { m_IsTrigger = isTrigger; }

		glm::vec2 GetPosition() const;
		
		glm::vec2 GetOffset() const { return m_Offset; }
		
		void SetOffset(const glm::vec2& offset) { m_Offset = offset; }

		virtual void SetSize(const glm::vec2& size) = 0;
		
		virtual ICollider2D* IntersectTrigger() = 0;
		
		virtual ~ICollider2D() {};
	};

}
