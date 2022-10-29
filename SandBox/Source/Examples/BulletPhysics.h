#pragma once

#include "Core/Core.h"
#include "Core/Application.h"
#include "BulletCommon/btBulletDynamicsCommon.h"

#include <vector>

using namespace Pengine;

class BulletPhysics : public Application
{
private:

	btDiscreteDynamicsWorld* m_World = nullptr;
	btAlignedObjectArray<btCollisionShape*> m_CollisionShapes;
	btAlignedObjectArray<btRigidBody*> m_RigidBodies;
	std::vector<GameObject*> m_GameObjects;

	btRigidBody* CreateRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape);
public:

	BulletPhysics(const std::string& title = "BulletPhysics")
		: Application(title)
	{}

	virtual void OnStart() override;

	virtual void OnPostStart() override {};

	virtual void OnUpdate() override;

	virtual void OnGuiRender() override {};

	virtual void OnImGuiRender() override {};

	virtual void OnClose() override;
};