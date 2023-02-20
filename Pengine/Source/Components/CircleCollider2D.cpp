#include "CircleCollider2D.h"

#include "Rigidbody2D.h"
#include "../Core/Scene.h"
#include "../Core/MemoryManager.h"
#include "../Core/Visualizer.h"

using namespace Pengine;

IComponent* CircleCollider2D::Create(GameObject* owner)
{
    CircleCollider2D* cc2d = MemoryManager::GetInstance().Allocate<CircleCollider2D>();
    glm::vec3 scale = owner->m_Transform.GetScale();
    cc2d->m_CircleShape.m_radius = cc2d->m_Radius * scale.x;
    cc2d->m_Shape = &cc2d->m_CircleShape;

    cc2d->m_Fixture.shape = &cc2d->m_CircleShape;
    cc2d->m_Fixture.density = cc2d->GetDensity();
    cc2d->m_Fixture.friction = cc2d->GetFriction();
    cc2d->m_Fixture.restitution = cc2d->GetRestitution();
    cc2d->m_Fixture.restitutionThreshold = cc2d->GetRestitutionThreshold();

    owner->GetScene()->m_CircleColliders2D.push_back(cc2d);

    return cc2d;
}

CircleCollider2D::CircleCollider2D(const CircleCollider2D& cc2d)
{
    Copy(cc2d);
}

CircleCollider2D& CircleCollider2D::operator=(const CircleCollider2D& cc2d)
{
    Copy(cc2d);

    return *this;
}

void CircleCollider2D::Copy(const IComponent& component)
{
    CircleCollider2D& cc2d = *(CircleCollider2D*)&component;
    m_Type = component.GetType();
    SetOffset(cc2d.GetOffset());
    SetRadius(cc2d.GetRadius());
    SetDensity(cc2d.GetDensity());
    SetFriction(cc2d.GetFriction());
    SetRestitution(cc2d.GetRestitution());
    SetRestitutionThreshold(cc2d.GetRestitutionThreshold());
    SetTag(cc2d.GetTag());
    SetTrigger(cc2d.IsTrigger());
}

void CircleCollider2D::Delete()
{
    Utils::Erase(m_Owner->GetScene()->m_CircleColliders2D, this);
    MemoryManager::GetInstance().FreeMemory<CircleCollider2D>(static_cast<IAllocator*>(this));
}

IComponent* CircleCollider2D::New(GameObject* owner)
{
    return Create(owner);
}

ICollider2D* CircleCollider2D::IntersectTrigger()
{
    Scene* scene = m_Owner->GetScene();

    size_t size = scene->m_CircleColliders2D.size();
    for (size_t i = 0; i < size; i++)
    {
        CircleCollider2D* cc2d = scene->m_CircleColliders2D[i];

        if (this == cc2d) continue;

        if (!cc2d->IsTrigger() || !cc2d->GetOwner()->IsEnabled()) continue;

        if (CircleCircleOverlap(cc2d))
        {
            return cc2d;
        }
    }

    size = scene->m_BoxColliders2D.size();
    for (size_t i = 0; i < size; i++)
    {
        BoxCollider2D* bc2d = scene->m_BoxColliders2D[i];
    
        if (!bc2d->IsTrigger() || !bc2d->GetOwner()->IsEnabled()) continue;
    
        if (bc2d->BoxCircleOverlap(this))
        {
            return bc2d;
        }
    }

    return nullptr;
}

bool CircleCollider2D::CircleCircleOverlap(CircleCollider2D* other)
{
    glm::vec3 scaleThis = m_Owner->m_Transform.GetScale();
    glm::vec3 scaleOther = other->m_Owner->m_Transform.GetScale();
    glm::vec3 positionThis = m_Owner->m_Transform.GetPosition();
    glm::vec3 positionOther = other->m_Owner->m_Transform.GetPosition();
    float distance = glm::sqrt(glm::pow(positionOther.x - positionThis.x, 2) + glm::pow(positionOther.y - positionThis.y, 2));

    if (distance < m_Radius * scaleThis.x + other->m_Radius * scaleOther.x)
    {
        return true;
    }
    else
    {
        return false;
    }
}
