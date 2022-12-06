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
	private:

		std::string m_Tag;

		glm::vec2 m_Offset = { 0.0f, 0.0f };

		float m_Density = 1.0f;
		float m_Friction = 0.5f;
		float m_Restitution = 0.0f;
		float m_RestitutionThreshold = 0.5f;

		bool m_IsTrigger = false;
	protected:

		b2Shape* m_Shape;
		b2FixtureDef m_Fixture;

		friend class Rigidbody2D;
	public:

		ICollider2D() { m_Type = "ICollider2D"; }
		
		void SetTrigger(bool isTrigger) { m_IsTrigger = isTrigger; }

		bool IsTrigger() const { return m_IsTrigger; }

		void SetTag(const std::string& tag) { m_Tag = tag; }

		std::string GetTag() const { return m_Tag; }

		void SetOffset(const glm::vec2& offset) { m_Offset = offset; }
		
		glm::vec2 GetOffset() const { return m_Offset; }

		void SetDensity(float density) { m_Density = density; }

		float GetDensity() const { return m_Density; }

		void SetFriction(float friction) { m_Friction = friction; }

		float GetFriction() const { return m_Friction; }

		void SetRestitution(float restitution) { m_Restitution = restitution; }

		float GetRestitution() const { return m_Restitution; }

		void SetRestitutionThreshold(float restitutionThreshold) { m_RestitutionThreshold = restitutionThreshold; }

		float GetRestitutionThreshold() const { return m_RestitutionThreshold; }

		glm::vec2 GetPosition() const;
		
		virtual ICollider2D* IntersectTrigger() = 0;
		
		virtual ~ICollider2D() {};
	};

}
