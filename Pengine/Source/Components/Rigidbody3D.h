#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"
#include "../Core/Allocator.h"
#include "ICollider3D.h"

namespace Pengine
{

	class PENGINE_API Rigidbody3D : public IComponent, public IAllocator
	{
	private:

		ICollider3D* m_CollisionShape = nullptr;
		btRigidBody m_Body = btRigidBody(btRigidBody::btRigidBodyConstructionInfo(0.0f, nullptr, nullptr));
		bool m_IsInitializedPhysics = false;

		void Initialize();

		friend class Scene;
	protected:

		virtual void Copy(const IComponent& component) override;

		virtual void Delete() override;

		virtual IComponent* New(GameObject* owner) override;
	public:

		float m_Mass = 0.0f;
		float m_Restitution = 0.0f;
		float m_Friction = 1.0f;
		bool m_AllowToSleep = true;

		static IComponent* Create(GameObject* owner);

		Rigidbody3D() = default;

		~Rigidbody3D() = default;

		void SetCollisionShape(ICollider3D* collisionShape);

		ICollider3D* GetCollisionShape() const { return m_CollisionShape; }

		void GetMassProps(btScalar& mass, btVector3& inertia);

		void SetActive(bool active);

		btRigidBody& GetBody() { return m_Body; }

		void ApplyCentralForce(const glm::vec3& force);
		
		void ApplyForce(const glm::vec3& force, const glm::vec3& relativePosition);

		void ApplyCentralImpulse(const glm::vec3& impulse);

		void ApplyImpulse(const glm::vec3& impulse, const glm::vec3& relativePosition);

		void ApplyCentralPushImpulse(const glm::vec3& impulse);

		void ApplyPushImpulse(const glm::vec3& impulse, const glm::vec3& relativePosition);

		void SetCollisionFlags(int flags);

		int GetCollisionFlags();

		bool IsInitializedPhysics() const { return m_IsInitializedPhysics; }
	};

}
