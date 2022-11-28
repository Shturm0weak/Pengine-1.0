#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"
#include "../Core/Allocator.h"
#include "BulletCommon/btBulletDynamicsCommon.h"

namespace Pengine
{

	class PENGINE_API ICollider3D : public IComponent, public IAllocator
	{
	protected:

		btCollisionShape* m_CollisionShape = nullptr;

		glm::vec3 m_Offset = { 0.0f, 0.0f, 0.0f };

		virtual void Initialize() = 0;

		friend class Rigidbody3D;
	public:

		ICollider3D() { m_Type = "ICollider3D"; }

		btCollisionShape* GetCollisionShape() const { return m_CollisionShape; }

		glm::vec3 GetOffset() const { return m_Offset; }

		void SetOffset(const glm::vec3& offset) { m_Offset = offset; }

		glm::vec3 GetPosition() const;

		virtual ~ICollider3D() {};
	};

}
