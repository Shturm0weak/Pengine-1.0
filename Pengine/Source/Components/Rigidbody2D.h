#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"
#include "../Core/Allocator.h"
#include "../OpenGL/Shader.h"
#include "Box2D/include/box2d/b2_body.h"

namespace Pengine
{

	class PENGINE_API Rigidbody2D : public IComponent, public IAllocator
	{
	private:

		enum class BodyType { Static = 0, Dynamic, Kinematic };

		b2Fixture* m_Fixture = nullptr;
		b2Body* m_Body = nullptr;
		BodyType m_BodyType = BodyType::Static;
		bool m_FixedRotation = false;

		friend class Scene;
		friend class Editor;
	public:

		Rigidbody2D() = default;

		static IComponent* Create(GameObject* owner);
		
		static b2BodyType ConvertToB2BodyType(BodyType bodyType);

		virtual IComponent* New(GameObject* owner) override;
		
		virtual void Copy(const IComponent& component) override;
		
		virtual void Delete() override;

		b2Body* GetBody() const { return m_Body; }
		
		void SetAllowToSleep(bool allowToSleep) { if (m_Body) m_Body->SetSleepingAllowed(allowToSleep); }
		
		bool IsSleepingAllowed() const { return m_Body ? m_Body->IsSleepingAllowed() : true; }
		
		void SetFixedRotation(bool fixedRotation);
		
		bool IsFixedRotation() const { return m_FixedRotation; }
		
		void SetStatic(bool isStatic);
		
		bool IsStatic() const { return m_BodyType == BodyType::Static; }
		
		void ApplyAngularImpulse(float impulse, bool wake);
		
		void ApplyForce(const glm::vec2& force, const glm::vec2& point, bool wake);
		
		void ApplyForceToCenter(const glm::vec2& force, bool wake);
		
		void ApplyLinearImpulse(const glm::vec2& impulse, const glm::vec2& point, bool wake);
		
		void ApplyLinearImpulseToCenter(const glm::vec2& impulse, bool wake);
		
		void ApplyTorque(float torque, bool wake);
		
		void SetLinearVelocity(const glm::vec2& velocity);

		void Initialize();
	};

}
