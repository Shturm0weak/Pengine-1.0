#include "BulletPhysicsWorld.h"

#include "Utils.h"

using namespace Pengine;

void BulletPhysicsWorld::Initialize(const glm::vec3& gravity)
{
	m_CollisionConfiguration = new btDefaultCollisionConfiguration();
	m_Dispatcher = new btCollisionDispatcher(m_CollisionConfiguration);
	m_Broadphase = new btDbvtBroadphase();
	m_Solver = new btSequentialImpulseConstraintSolver;
	m_World = new btDiscreteDynamicsWorld(m_Dispatcher, m_Broadphase, m_Solver, m_CollisionConfiguration);
	m_World->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

void BulletPhysicsWorld::UnInitialize()
{
	delete m_World;
	delete m_Solver;
	delete m_Broadphase;
	delete m_Dispatcher;
	delete m_CollisionConfiguration;
	m_World = nullptr;
}

void BulletPhysicsWorld::Step(float deltaTime)
{
	if (m_World)
	{
		m_World->stepSimulation(deltaTime, m_MaxSubSteps);
	}
}

void BulletPhysicsWorld::AddRigidbody(btRigidBody* rigidbody)
{
	rigidbody->setUserIndex(-1);
	m_World->addRigidBody(rigidbody);
	m_RigidBodies.push_back(rigidbody);
}

void BulletPhysicsWorld::RemoveRigidbody(btRigidBody* rigidbody)
{
	m_World->removeRigidBody(rigidbody);
	Utils::Erase<btRigidBody*>(m_RigidBodies, rigidbody);
}