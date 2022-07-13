#include "BoxCollider2D.h"

#include "Rigidbody2D.h"
#include "CircleCollider2D.h"
#include "../Core/Scene.h"
#include "../Core/MemoryManager.h"
#include "../Core/Visualizer.h"
#include "../Core/Raycast2D.h"

using namespace Pengine;

bool BoxCollider2D::BoxBoxOverlapSAT(BoxCollider2D* other)
{
    std::vector<glm::vec4> thisBox(4);
    std::vector<glm::vec4> otherBox(4);

    Transform transform = m_Owner->m_Transform;
    transform.Translate({ GetPosition(), 0.0f });
    glm::mat4 transformMat = transform.GetTransform();
    thisBox[0] = { transformMat * glm::vec4(-m_Size.x, -m_Size.y, 0.0f, 1.0f) };
    thisBox[1] = { transformMat * glm::vec4( m_Size.x, -m_Size.y, 0.0f, 1.0f) };
    thisBox[2] = { transformMat * glm::vec4( m_Size.x,  m_Size.y, 0.0f, 1.0f) };
    thisBox[3] = { transformMat * glm::vec4(-m_Size.x,  m_Size.y, 0.0f, 1.0f) };

    transform = other->m_Owner->m_Transform;
    transform.Translate({ other->GetPosition(), 0.0f });
    transformMat = transform.GetTransform();
    otherBox[0] = { transformMat * glm::vec4(-other->m_Size.x, -other->m_Size.y, 0.0f, 1.0f) };
    otherBox[1] = { transformMat * glm::vec4( other->m_Size.x, -other->m_Size.y, 0.0f, 1.0f) };
    otherBox[2] = { transformMat * glm::vec4( other->m_Size.x,  other->m_Size.y, 0.0f, 1.0f) };
    otherBox[3] = { transformMat * glm::vec4(-other->m_Size.x,  other->m_Size.y, 0.0f, 1.0f) };

    std::vector<glm::vec4> box1 = thisBox;
    std::vector<glm::vec4> box2 = otherBox;

    float overlap = INFINITY;

    for (int shape = 0; shape < 2; shape++)
    {
        if (shape == 1)
        {
            box1 = otherBox;
            box2 = thisBox;
        }

        for (int a = 0; a < 4; a++)
        {
            int b = (a + 1) % 4;
            glm::vec2 axisProj = glm::vec2(-(box1[b].y - box1[a].y), box1[b].x - box1[a].x);

            // Optional normalisation of projection axis enhances stability slightly
            //float d = sqrtf(axisProj.x * axisProj.x + axisProj.y * axisProj.y);
            //axisProj = { axisProj.x / d, axisProj.y / d };

            float minR1 = INFINITY, maxR1 = -INFINITY;

            for (int p = 0; p < 4; p++)
            {
                float q = (box1[p].x * axisProj.x + box1[p].y * axisProj.y);
                minR1 = std::fmin(minR1, q);
                maxR1 = std::fmax(maxR1, q);
            }

            float minR2 = INFINITY, maxR2 = -INFINITY;

            for (int p = 0; p < 4; p++)
            {
                float q = (box2[p].x * axisProj.x + box2[p].y * axisProj.y);
                minR2 = std::fmin(minR2, q);
                maxR2 = std::fmax(maxR2, q);
            }

            overlap = std::fmin(std::fmin(maxR1, maxR2) - std::fmax(minR1, minR2), overlap);

            if (!(maxR2 >= minR1 && maxR1 >= minR2))
            {
                return false;
            }
        }
    }

    return true;
}

bool BoxCollider2D::BoxCircleOverlap(CircleCollider2D* other)
{
    std::vector<glm::vec4> thisBox(4);

    Transform transform = m_Owner->m_Transform;
    transform.Translate({ GetPosition(), 0.0f });
    glm::mat4 transformMat = transform.GetTransform();
    thisBox[0] = { transformMat * glm::vec4(-m_Size.x, -m_Size.y, 0.0f, 1.0f) };
    thisBox[1] = { transformMat * glm::vec4( m_Size.x, -m_Size.y, 0.0f, 1.0f) };
    thisBox[2] = { transformMat * glm::vec4( m_Size.x,  m_Size.y, 0.0f, 1.0f) };
    thisBox[3] = { transformMat * glm::vec4(-m_Size.x,  m_Size.y, 0.0f, 1.0f) };

    glm::vec3 scaleOther = other->GetOwner()->m_Transform.GetScale();
    glm::vec3 positionOther = { other->GetPosition(), 0.0f };
    float radius = other->GetRadius() * scaleOther.x;

    glm::vec2 result = { 0.0f, 0.0f };

    if (Raycast2D::CircleIntersect(thisBox[0], thisBox[1], positionOther, radius, result))
    {
        return true;
    }
    else if (Raycast2D::CircleIntersect(thisBox[1], thisBox[2], positionOther, radius, result))
    {
        return true;
    }
    else if (Raycast2D::CircleIntersect(thisBox[2], thisBox[3], positionOther, radius, result))
    {
        return true;
    }
    else if (Raycast2D::CircleIntersect(thisBox[3], thisBox[0], positionOther, radius, result))
    {
        return true;
    }

    return false;
}

IComponent* BoxCollider2D::Create(GameObject* owner)
{
    BoxCollider2D* bc2d = MemoryManager::GetInstance().Allocate<BoxCollider2D>();
    glm::vec3 scale = owner->m_Transform.GetScale();
    bc2d->m_PolygonShape.SetAsBox(bc2d->m_Size.x * scale.x, bc2d->m_Size.y * scale.y);
    bc2d->m_Shape = &bc2d->m_PolygonShape;

    bc2d->m_Fixture.shape = &bc2d->m_PolygonShape;
    bc2d->m_Fixture.density = bc2d->m_Density;
    bc2d->m_Fixture.friction = bc2d->m_Friction;
    bc2d->m_Fixture.restitution = bc2d->m_Restitution;
    bc2d->m_Fixture.restitutionThreshold = bc2d->m_RestitutionThreshold;

    owner->GetScene()->m_BoxColliders2D.push_back(bc2d);

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
    m_Tag = bc2d.m_Tag;
    m_IsTrigger = bc2d.m_IsTrigger;
}

void BoxCollider2D::Delete()
{
    Utils::Erase(m_Owner->GetScene()->m_BoxColliders2D, this);
    MemoryManager::GetInstance().FreeMemory<BoxCollider2D>(static_cast<IAllocator*>(this));
}

ICollider2D* BoxCollider2D::IntersectTrigger()
{
    Scene* scene = m_Owner->GetScene();

    size_t size = scene->m_BoxColliders2D.size();
    for (size_t i = 0; i < size; i++)
    {
        BoxCollider2D* bc2d = scene->m_BoxColliders2D[i];
        
        if (this == bc2d) continue;

        if (!bc2d->m_IsTrigger || !bc2d->GetOwner()->m_IsEnabled) continue;

        if (BoxBoxOverlapSAT(bc2d))
        {
            return bc2d;
        }
    }

    size = scene->m_CircleColliders2D.size();
    for (size_t i = 0; i < size; i++)
    {
        CircleCollider2D* cc2d = scene->m_CircleColliders2D[i];

        if (!cc2d->m_IsTrigger || !cc2d->GetOwner()->m_IsEnabled) continue;

        if (BoxCircleOverlap(cc2d))
        {
            return cc2d;
        }
    }

    return nullptr;
}

glm::vec2 BoxCollider2D::GetPosition() const
{
    return m_Offset + glm::vec2(m_Owner->m_Transform.GetPosition());
}

IComponent* BoxCollider2D::New(GameObject* owner)
{
    return Create(owner);
}
