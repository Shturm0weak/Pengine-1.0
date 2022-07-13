#include "ICollider2D.h"

#include "../Core/GameObject.h"

using namespace Pengine;

glm::vec2 ICollider2D::GetPosition() const
{
	return m_Offset + glm::vec2(m_Owner->m_Transform.GetPosition());
}
