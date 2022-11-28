#include "Rigidbody3D.h"

#include "../Core/Scene.h"
#include "../Core/MemoryManager.h"

using namespace Pengine;

IComponent* Rigidbody3D::Create(GameObject* owner)
{
	Rigidbody3D* rb3d = MemoryManager::GetInstance().Allocate<Rigidbody3D>();
	owner->GetScene()->m_Rigidbody3D.push_back(rb3d);

	return rb3d;
}
IComponent* Rigidbody3D::New(GameObject* owner)
{
	return Create(owner);
}

void Rigidbody3D::Copy(const IComponent& component)
{
	Rigidbody3D& rb3d = *(Rigidbody3D*)&component;
	m_Type = component.GetType();
	m_Mass = rb3d.m_Mass;
	m_Restitution = rb3d.m_Restitution;
	m_Friction = rb3d.m_Friction;
	m_AllowToSleep = rb3d.m_AllowToSleep;
}

void Rigidbody3D::Delete()
{
	GetOwner()->GetScene()->GetBulletPhysicsWorld().RemoveRigidbody(&m_Body);

	Utils::Erase(m_Owner->GetScene()->m_Rigidbody3D, this);
	MemoryManager::GetInstance().FreeMemory<Rigidbody3D>(static_cast<IAllocator*>(this));
}

void Rigidbody3D::SetCollisionShape(ICollider3D* collisionShape)
{
	m_CollisionShape = collisionShape;
}

void Rigidbody3D::Initialize()
{
	if (m_IsInitializedPhysics) return;

	m_IsInitializedPhysics = true;

	SetCollisionShape(GetOwner()->m_ComponentManager.GetComponentSubClass<ICollider3D>());
	
	if (!m_CollisionShape)
	{
		return;
	}

	m_CollisionShape->Initialize();

	btCollisionShape* collisionShape = m_CollisionShape->GetCollisionShape();

	if (!collisionShape || collisionShape->getShapeType() == INVALID_SHAPE_PROXYTYPE)
	{
		return;
	}

	btVector3 inertia = { 0.0f, 0.0f, 0.0f };
	GetMassProps(m_Mass, inertia);

	glm::vec3 position = m_CollisionShape->GetPosition();
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin({ position.x, position.y, position.z });
	btDefaultMotionState* motionState = new btDefaultMotionState(transform);

	btRigidBody::btRigidBodyConstructionInfo constructInfo(m_Mass, motionState, 
		collisionShape, inertia);

	BulletPhysicsWorld& world = GetOwner()->GetScene()->GetBulletPhysicsWorld();
	m_Body = btRigidBody(constructInfo);
	world.AddRigidbody(&m_Body);
}

void Rigidbody3D::GetMassProps(btScalar& mass, btVector3& inertia)
{
	mass = m_Mass;
	inertia = { 0.0f, 0.0f, 0.0f };

	bool isDynamic = (m_Mass != 0.f);
	if (isDynamic && m_CollisionShape->GetCollisionShape())
	{
		m_CollisionShape->GetCollisionShape()->calculateLocalInertia(m_Mass, inertia);
	}
}

void Rigidbody3D::SetActive(bool active)
{
	if (m_IsInitializedPhysics)
	{
		m_Body.activate(active);
	}
}

void Rigidbody3D::ApplyCentralForce(const glm::vec3& force)
{
	if (m_IsInitializedPhysics)
	{
		m_Body.applyCentralForce({ force.x, force.y, force.z });
	}
}

void Rigidbody3D::ApplyForce(const glm::vec3& force, const glm::vec3& relativePosition)
{
	if (m_IsInitializedPhysics)
	{
		m_Body.applyForce({ force.x, force.y, force.z }, { relativePosition.x, relativePosition.y, relativePosition.z });
	}
}

void Rigidbody3D::ApplyCentralImpulse(const glm::vec3& impulse)
{
	if (m_IsInitializedPhysics)
	{
		m_Body.applyCentralImpulse({ impulse.x, impulse.y, impulse.z });
	}
}

void Rigidbody3D::ApplyImpulse(const glm::vec3& impulse, const glm::vec3& relativePosition)
{
	if (m_IsInitializedPhysics)
	{
		m_Body.applyImpulse({ impulse.x, impulse.y, impulse.z }, { relativePosition.x, relativePosition.y, relativePosition.z });
	}
}

void Rigidbody3D::ApplyCentralPushImpulse(const glm::vec3& impulse)
{
	if (m_IsInitializedPhysics)
	{
		m_Body.applyCentralPushImpulse({ impulse.x, impulse.y, impulse.z });
	}
}

void Rigidbody3D::ApplyPushImpulse(const glm::vec3& impulse, const glm::vec3& relativePosition)
{
	if (m_IsInitializedPhysics)
	{
		m_Body.applyPushImpulse({ impulse.x, impulse.y, impulse.z }, { relativePosition.x, relativePosition.y, relativePosition.z });
	}
}

void Rigidbody3D::SetCollisionFlags(int flags)
{
	if (m_IsInitializedPhysics)
	{
		m_Body.setCollisionFlags(flags);
	}
}

int Rigidbody3D::GetCollisionFlags()
{
	if (m_IsInitializedPhysics)
	{
		return m_Body.getCollisionFlags();
	}
	else
	{
		return 0;
	}
}

