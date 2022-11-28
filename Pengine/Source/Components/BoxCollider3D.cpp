#include "BoxCollider3D.h"

#include "../Core/Scene.h"
#include "../Core/MemoryManager.h"

using namespace Pengine;

IComponent* BoxCollider3D::Create(GameObject* owner)
{
    BoxCollider3D* bc3d = MemoryManager::GetInstance().Allocate<BoxCollider3D>();
    owner->GetScene()->m_BoxColliders3D.push_back(bc3d);

    owner->m_Transform.SetOnScaleCallback("BoxCollider3D", 
        [bc3d]()
    {
        if (IComponent::IsValid(bc3d) && bc3d->m_IsInitialized)
        {
            bc3d->SetHalfExtent(bc3d->GetHalfExtent());
        }
    });

    return bc3d;
}
void BoxCollider3D::Copy(const IComponent& component)
{
    BoxCollider3D& bc3d = *(BoxCollider3D*)&component;
    m_Type = component.GetType();
    SetHalfExtent(bc3d.GetHalfExtent());
    SetOffset(bc3d.GetOffset());
}

void BoxCollider3D::Delete()
{
    if (m_CollisionShape)
    {
        delete m_CollisionShape;
    }

    GetOwner()->m_Transform.RemoveOnScaleCallback("BoxCollider3D");
    Utils::Erase(m_Owner->GetScene()->m_BoxColliders3D, this);
    MemoryManager::GetInstance().FreeMemory<BoxCollider3D>(static_cast<IAllocator*>(this));
}

IComponent* BoxCollider3D::New(GameObject* owner)
{
    return Create(owner);
}

void BoxCollider3D::SetHalfExtent(const glm::vec3& halfExtent)
{
    if (halfExtent.x == 0.0f || halfExtent.y == 0.0f || halfExtent.z == 0.0f)
    {
        return;
    }

    m_HalfExtent = halfExtent;
    if (GameObject* owner = GetOwner())
    {
        glm::vec3 ownerScale = owner->m_Transform.GetScale();
        if (ownerScale.x == 0.0f || ownerScale.y == 0.0f || ownerScale.z == 0.0f)
        {
            return;
        }

        const glm::vec3 localScale = ownerScale * m_HalfExtent;
        if (m_CollisionShape)
        {
            m_CollisionShape->setLocalScaling({ localScale.x, localScale.y, localScale.z });
        }
    }
}

void BoxCollider3D::Initialize()
{
    m_CollisionShape = new btBoxShape({ 1.0f, 1.0f, 1.0f });
    SetHalfExtent(GetHalfExtent());
}
