#include "BulletPhysics.h"

#include "Core/Visualizer.h"

btRigidBody* BulletPhysics::CreateRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape)
{
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

	btRigidBody* body = new btRigidBody(cInfo);

	body->setUserIndex(-1);
	m_World->addRigidBody(body);
	m_RigidBodies.push_back(body);

	GameObject* gameObject = GetScene()->CreateGameObject("Cube");
	m_GameObjects.push_back(gameObject);
	Renderer3D* r3d = gameObject->m_ComponentManager.AddComponent<Renderer3D>();
	MeshManager::GetInstance().LoadAsync("Source/Meshes/Sphere.meta",
		[r3d](Mesh* mesh)
	{
		r3d->SetMesh(mesh);
	});

	return body;
}

void BulletPhysics::OnStart()
{
	///collision configuration contains default setup for memory, collision setup
	btDefaultCollisionConfiguration* m_collisionConfiguration = new btDefaultCollisionConfiguration();
	//m_collisionConfiguration->setConvexConvexMultipointIterations();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

	btDbvtBroadphase* m_broadphase = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
	btSequentialImpulseConstraintSolver* m_solver = sol;

	m_World = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
	m_World->setGravity(btVector3(0.0f, -9.8f, 0.0f));

	///create a few basic rigid bodies
	btSphereShape* groundShape = new btSphereShape(100.0f);

	//groundShape->initializePolyhedralFeatures();
	//btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),50);

	m_CollisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -100, 0));

	{
		btScalar mass(0.);
		CreateRigidBody(mass, groundTransform, groundShape);
	}

	{
		//create a few dynamic rigidbodies
		// Re-using the same collision is better for memory usage and performance

		btSphereShape* colShape = new btSphereShape(1.0f);

		//btCollisionShape* colShape = new btSphereShape(btScalar(1.));
		m_CollisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(1.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		for (int k = 0; k < 5; k++)
		{
			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < 5; j++)
				{
					startTransform.setOrigin(btVector3(
						btScalar(0.2 * i),
						btScalar(2 + .2 * k),
						btScalar(0.2 * j)));

					CreateRigidBody(mass, startTransform, colShape);
				}
			}
		}
	}
}

void BulletPhysics::OnUpdate()
{
	for (size_t i = 0; i < m_RigidBodies.size(); i++)
	{
		btRigidBody* rb = m_RigidBodies[i];
		GameObject* gameObject = m_GameObjects[i];
		btSphereShape* col = static_cast<btSphereShape*>(rb->getCollisionShape());
		//btVector3 scale = box->getHalfExtentsWithoutMargin();
		float scale = col->getRadius();
		//glm::vec3 max = { scale.x(), scale.y(), scale.z() };
		glm::vec3 max = { scale, scale, scale };
		glm::mat4 transformMat4;
		rb->getWorldTransform().getOpenGLMatrix(glm::value_ptr(transformMat4));

		gameObject->m_Transform.Translate(Utils::GetPosition(transformMat4));
		gameObject->m_Transform.Scale(max);
		gameObject->m_Transform.SetRotationMat4(glm::mat4(glm::mat3(transformMat4)));

		/*Visualizer::DrawWireFrameCube(glm::translate(glm::mat4(1.0f), Utils::GetPosition(transformMat4)),
			glm::mat4(glm::mat3(transformMat4)),
			glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f, 1.0f }), 
			-max, max);*/
	}

	if (m_World)
	{
		m_World->stepSimulation(Time::GetDeltaTime());
	}
}

void BulletPhysics::OnClose()
{
	if (m_World)
	{
		int i;
		for (i = m_World->getNumConstraints() - 1; i >= 0; i--)
		{
			m_World->removeConstraint(m_World->getConstraint(i));
		}
		for (i = m_World->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = m_World->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			m_World->removeCollisionObject(obj);
			delete obj;
		}
	}
	//delete collision shapes
	for (int j = 0; j < m_CollisionShapes.size(); j++)
	{
		btCollisionShape* shape = m_CollisionShapes[j];
		delete shape;
	}
	m_CollisionShapes.clear();

	m_RigidBodies.clear();
	m_GameObjects.clear();

	delete m_World;
	m_World = 0;
}
