#include "BoxCollider2D.h"

#include "Rigidbody2D.h"
#include "../Core/Scene.h"
#include "../Core/MemoryManager.h"

using namespace Pengine;

IComponent* BoxCollider2D::Create(GameObject* owner)
{
    BoxCollider2D* bc2d = MemoryManager::GetInstance().Allocate<BoxCollider2D>();
    glm::vec3 scale = owner->m_Transform.GetScale();
    bc2d->m_PolygonShape.SetAsBox(bc2d->m_Size.x * scale.x, bc2d->m_Size.y * scale.y);

    bc2d->m_Fixture.shape = &bc2d->m_PolygonShape;
    bc2d->m_Fixture.density = bc2d->m_Density;
    bc2d->m_Fixture.friction = bc2d->m_Friction;
    bc2d->m_Fixture.restitution = bc2d->m_Restitution;
    bc2d->m_Fixture.restitutionThreshold = bc2d->m_RestitutionThreshold;

    return bc2d;
}

void BoxCollider2D::Copy(const IComponent& component)
{
    BoxCollider2D& bc2d = *(BoxCollider2D*)&component;
    m_Offset = bc2d.m_Offset;
    m_Size = bc2d.m_Size;
    m_Density = bc2d.m_Density;
    m_Friction = bc2d.m_Friction;
    m_Restitution = bc2d.m_Restitution;
    m_RestitutionThreshold = bc2d.m_RestitutionThreshold;
    m_Type = component.GetType();
}

IComponent* BoxCollider2D::CreateCopy(GameObject* newOwner)
{
    BoxCollider2D* bc2d = (BoxCollider2D*)Create(newOwner);
    *bc2d = *this;
    return bc2d;
}

void BoxCollider2D::Delete()
{
    MemoryManager::GetInstance().FreeMemory<BoxCollider2D>(static_cast<IAllocator*>(this));
}

void BoxCollider2D::operator=(const BoxCollider2D& bc2d)
{
    Copy(bc2d);
}
