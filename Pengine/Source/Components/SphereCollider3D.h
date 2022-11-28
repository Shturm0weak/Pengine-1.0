#pragma once

#include "../Core/Core.h"
#include "ICollider3D.h"

namespace Pengine
{

	class PENGINE_API SphereCollider3D : public ICollider3D
	{
	private:

		float m_Radius = 1.0f;

		virtual void Initialize() override;
	public:

		SphereCollider3D() = default;
		~SphereCollider3D() = default;

		static IComponent* Create(GameObject* owner);

		virtual void Copy(const IComponent& component) override;

		virtual void Delete() override;

		virtual IComponent* New(GameObject* owner) override;

		void SetRadius(float radius);

		float GetRadius() const { return m_Radius; }
	};

}
