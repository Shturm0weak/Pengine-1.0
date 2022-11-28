#pragma once

#include "Core.h"
#include "BulletCommon/btBulletDynamicsCommon.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API BulletPhysicsWorld
	{
	private:

		std::vector<btRigidBody*> m_RigidBodies;
		btDiscreteDynamicsWorld* m_World = nullptr;

		btDefaultCollisionConfiguration* m_CollisionConfiguration = nullptr;
		btCollisionDispatcher* m_Dispatcher = nullptr;
		btDbvtBroadphase* m_Broadphase = nullptr;
		btSequentialImpulseConstraintSolver* m_Solver = nullptr;

		int m_MaxSubSteps = 16;
	public:

		BulletPhysicsWorld() = default;
		~BulletPhysicsWorld() = default;

		void Initialize(const glm::vec3& gravity = glm::vec3(0.0f, -9.8f, 0.0f));

		void UnInitialize();

		void Step(float deltaTime);

		void AddRigidbody(btRigidBody* rigidbody);

		void RemoveRigidbody(btRigidBody* rigidbody);
	};

}
