#include "Rigidbody2D.h"

#include "BoxCollider2D.h"
#include "../Core/Scene.h"
#include "../Core/MemoryManager.h"
#include "Box2D/include/box2d/b2_world.h"

using namespace Pengine;

IComponent* Rigidbody2D::Create(GameObject* owner)
{
	Rigidbody2D* rb2d = MemoryManager::GetInstance().Allocate<Rigidbody2D>();
	owner->GetScene()->m_Rigidbody2D.insert(std::make_pair(rb2d->GetUUID(), rb2d));
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

IComponent* Rigidbody2D::CreateCopy(GameObject* newOwner)
{
	Rigidbody2D* rb2d = (Rigidbody2D*)Create(newOwner);
	rb2d->m_Type = Utils::GetTypeName<Rigidbody2D>();
	*rb2d = *this;
	return rb2d;
}

void Rigidbody2D::Delete()
{
	Utils::Erase(m_Owner->GetScene()->m_Rigidbody2D, GetUUID());
	MemoryManager::GetInstance().FreeMemory<Rigidbody2D>(static_cast<IAllocator*>(this));
}

void Rigidbody2D::operator=(const Rigidbody2D& rb2d)
{
	m_BodyType = rb2d.m_BodyType;
	m_FixedRotation = rb2d.m_FixedRotation;
	m_Type = rb2d.GetType();
}

void Rigidbody2D::Initialize()
{
	b2World& box2World = m_Owner->GetScene()->GetBox2DWorld();
	b2BodyDef bodyDef;
	bodyDef.type = Rigidbody2D::ConvertToB2BodyType(m_BodyType);
	glm::vec3 position = m_Owner->m_Transform.GetPosition();
	bodyDef.position.Set(position.x, position.y);
	bodyDef.angle = m_Owner->m_Transform.GetRotation().z;
	b2Body* body = box2World.CreateBody(&bodyDef);
	body->SetFixedRotation(m_FixedRotation);
	m_Body = body;
	m_Fixture = m_Body->CreateFixture(&m_Owner->m_ComponentManager.GetComponent<BoxCollider2D>()->m_Fixture);
}