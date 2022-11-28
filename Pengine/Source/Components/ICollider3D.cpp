#include "ICollider3D.h"

#include "../Core/GameObject.h"

using namespace Pengine;

glm::vec3 ICollider3D::GetPosition() const
{
	return GetOwner()->m_Transform.GetPosition() + m_Offset;
}