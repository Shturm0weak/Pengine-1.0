#include "CircleCollider2D.h"

#include "Rigidbody2D.h"
#include "../Core/Scene.h"
#include "../Core/MemoryManager.h"
#include "../Core/Visualizer.h"

using namespace Pengine;

bool CircleCollider2D::CircleCircleOberlap(CircleCollider2D* other)
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

IComponent* CircleCollider2D::Create(GameObject* owner)
{
    CircleCollider2D* cc2d = MemoryManager::GetInstance().Allocate<CircleCollider2D>();
    glm::vec3 scale = owner->m_Transform.GetScale();
    cc2d->m_CircleShape.m_radius = cc2d->m_Radius * scale.x;
    cc2d->m_Shape = &cc2d->m_CircleShape;

    cc2d->m_Fixture.shape = &cc2d->m_CircleShape;
    cc2d->m_Fixture.density = cc2d->m_Density;
    cc2d->m_Fixture.friction = cc2d->m_Friction;
    cc2d->m_Fixture.restitution = cc2d->m_Restitution;
    cc2d->m_Fixture.restitutionThreshold = cc2d->m_RestitutionThreshold;

    owner->GetScene()->m_CircleColliders2D.push_back(cc2d);

    return cc2d;
}

void CircleCollider2D::Copy(const IComponent& component)
{
    CircleCollider2D& cc2d = *(CircleCollider2D*)&component;
    m_Offset = cc2d.m_Offset;
    m_Radius = cc2d.m_Radius;
    m_Density = cc2d.m_Density;
    m_Friction = cc2d.m_Friction;
    m_Restitution = cc2d.m_Restitution;
    m_RestitutionThreshold = cc2d.m_RestitutionThreshold;
    m_Type = component.GetType();
    m_Tag = cc2d.m_Tag;
    m_IsTrigger = cc2d.m_IsTrigger;
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

        if (!cc2d->m_IsTrigger || !cc2d->GetOwner()->IsEnabled()) continue;

        if (CircleCircleOberlap(cc2d))
        {
            return cc2d;
        }
    }

    size = scene->m_BoxColliders2D.size();
    for (size_t i = 0; i < size; i++)
    {
        BoxCollider2D* bc2d = scene->m_BoxColliders2D[i];
    
        if (!bc2d->m_IsTrigger || !bc2d->GetOwner()->IsEnabled()) continue;
    
        if (bc2d->BoxCircleOverlap(this))
        {
            return bc2d;
        }
    }

    return nullptr;
}
