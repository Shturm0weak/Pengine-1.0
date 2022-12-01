#include "SphereCollider3D.h"

#include "../Core/Scene.h"
#include "../Core/MemoryManager.h"

using namespace Pengine;

IComponent* SphereCollider3D::Create(GameObject* owner)
{
    SphereCollider3D* bc3d = MemoryManager::GetInstance().Allocate<SphereCollider3D>();
    owner->GetScene()->m_SphereColliders3D.push_back(bc3d);

    owner->m_Transform.SetOnScaleCallback("SphereCollider3D", 
        [bc3d]()
    {
        if (IComponent::IsValid(bc3d) && bc3d->IsInitialized())
        {
            bc3d->SetRadius(bc3d->GetRadius());
        }
    });

    return bc3d;
}
void SphereCollider3D::Copy(const IComponent& component)
{
    SphereCollider3D& bc3d = *(SphereCollider3D*)&component;
    m_Type = component.GetType();
    SetRadius(bc3d.GetRadius());
    SetOffset(bc3d.GetOffset());
}

void SphereCollider3D::Delete()
{
    if (m_CollisionShape)
    {
        delete m_CollisionShape;
    }

    GetOwner()->m_Transform.RemoveOnScaleCallback("SphereCollider3D");
    Utils::Erase(m_Owner->GetScene()->m_SphereColliders3D, this);
    MemoryManager::GetInstance().FreeMemory<SphereCollider3D>(static_cast<IAllocator*>(this));
}

IComponent* SphereCollider3D::New(GameObject* owner)
{
    return Create(owner);
}

void SphereCollider3D::SetRadius(float radius)
{
    if (radius == 0.0f)
    {
        return;
    }

    m_Radius = radius;
    if (GameObject* owner = GetOwner())
    {
        glm::vec3 ownerScale = owner->m_Transform.GetScale();
        if (ownerScale.x == 0 || ownerScale.y == 0 || ownerScale.z == 0)
        {
            return;
        }

        const glm::vec3 localScale = ownerScale * m_Radius;
        if (m_CollisionShape)
        {
            m_CollisionShape->setLocalScaling({ localScale.x, localScale.y, localScale.z });
        }
    }
}

void SphereCollider3D::Initialize()
{
    m_CollisionShape = new btSphereShape({ 1.0f });
    SetRadius(GetRadius());
}
