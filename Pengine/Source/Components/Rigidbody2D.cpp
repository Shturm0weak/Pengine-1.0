#include "Rigidbody2D.h"

#include "BoxCollider2D.h"
#include "../Core/Scene.h"
#include "../Core/MemoryManager.h"
#include "Box2D/include/box2d/b2_world.h"
#include "Box2D/include/box2d/b2_contact.h"

using namespace Pengine;

IComponent* Rigidbody2D::Create(GameObject* owner)
{
	Rigidbody2D* rb2d = MemoryManager::GetInstance().Allocate<Rigidbody2D>();
	owner->GetScene()->m_Rigidbody2D.push_back(rb2d);

	return rb2d;
}

b2BodyType Rigidbody2D::ConvertToB2BodyType(BodyType bodyType)
{
	switch (bodyType)
	{
		case BodyType::Dynamic:   return b2_dynamicBody;
		case BodyType::Static:    return b2_staticBody;
		case BodyType::Kinematic: return b2_kinematicBody;
	}
}

void Rigidbody2D::Copy(const IComponent& component)
{
	Rigidbody2D& rb2d = *(Rigidbody2D*)&component;
	m_BodyType = rb2d.m_BodyType;
	m_FixedRotation = rb2d.m_FixedRotation;
	m_Type = rb2d.GetType();
}

IComponent* Rigidbody2D::New(GameObject* owner)
{
	return Create(owner);
}

void Rigidbody2D::Delete()
{
	if (m_Body)
	{
		m_Body->DestroyFixture(m_Fixture);
		m_Owner->GetScene()->m_Box2DWorld->DestroyBody(m_Body);
	}

	Utils::Erase(m_Owner->GetScene()->m_Rigidbody2D, this);
	MemoryManager::GetInstance().FreeMemory<Rigidbody2D>(static_cast<IAllocator*>(this));
}

void Rigidbody2D::SetFixedRotation(bool fixedRotation)
{
	m_FixedRotation = fixedRotation;

	if (m_Body)
	{
		m_Body->SetFixedRotation(m_FixedRotation);
	}
}

void Rigidbody2D::SetStatic(bool isStatic)
{ 
	m_BodyType = isStatic ? Rigidbody2D::BodyType::Static : Rigidbody2D::BodyType::Dynamic;
	
	if (m_Body)
	{
		m_Body->SetType(ConvertToB2BodyType(m_BodyType));
	}
}

void Rigidbody2D::Initialize()
{
	if (m_IsInitializedPhysics) return;

	if (ICollider2D* c2d = m_Owner->m_ComponentManager.GetComponentSubClass<ICollider2D>())
	{
		b2World& box2World = m_Owner->GetScene()->GetBox2DWorld();
		b2BodyDef bodyDef;
		bodyDef.type = Rigidbody2D::ConvertToB2BodyType(m_BodyType);
		glm::vec3 position = m_Owner->m_Transform.GetPosition();
		bodyDef.position.Set(position.x, position.y);
		bodyDef.angle = m_Owner->m_Transform.GetRotation().z;
		m_Body = box2World.CreateBody(&bodyDef);
		m_Body->SetFixedRotation(m_FixedRotation);
		m_Fixture = m_Body->CreateFixture(&c2d->m_Fixture);
		m_Body->SetTransform({ c2d->GetPosition().x, c2d->GetPosition().y }, m_Owner->m_Transform.GetRotation().z);
		m_IsInitializedPhysics = true;
	}
}

bool Rigidbody2D::IsCollided() const
{
	if (m_Body && m_Body->GetContactList())
	{
		return m_Body->GetContactList()->contact->IsTouching();
	}
	
	return false;
}

void Rigidbody2D::ApplyAngularImpulse(float impulse, bool wake)
{
	if (m_Body)
	{
		m_Body->ApplyAngularImpulse(impulse, wake);
	}
}

void Rigidbody2D::ApplyForce(const glm::vec2& force, const glm::vec2& point, bool wake)
{
	if (m_Body)
	{
		m_Body->ApplyForce({ force.x, force.y }, { point.x, point.y }, wake);
	}
}

void Rigidbody2D::ApplyForceToCenter(const glm::vec2& force, bool wake)
{
	if (m_Body)
	{
		m_Body->ApplyForceToCenter({ force.x, force.y }, wake);
	}
}

void Rigidbody2D::ApplyLinearImpulse(const glm::vec2& impulse, const glm::vec2& point, bool wake)
{
	if (m_Body)
	{
		m_Body->ApplyLinearImpulse({ impulse.x, impulse.y }, { point.x, point.y }, wake);
	}
}

void Rigidbody2D::ApplyLinearImpulseToCenter(const glm::vec2& impulse, bool wake)
{
	if (m_Body)
	{
		m_Body->ApplyLinearImpulseToCenter({ impulse.x, impulse.y }, wake);
	}
}

void Rigidbody2D::ApplyTorque(float torque, bool wake)
{
	if (m_Body)
	{
		m_Body->ApplyTorque(torque, wake);
	}
}

void Rigidbody2D::SetLinearVelocity(const glm::vec2& velocity)
{
	if (m_Body)
	{
		m_Body->SetLinearVelocity({ velocity.x, velocity.y });
	}
}
