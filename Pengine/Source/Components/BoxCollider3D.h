#pragma once

#include "../Core/Core.h"
#include "ICollider3D.h"

namespace Pengine
{

	class PENGINE_API BoxCollider3D : public ICollider3D
	{
	private:

		glm::vec3 m_HalfExtent = { 1.0f, 1.0f, 1.0f };

		virtual void Initialize() override;
	protected:

		virtual void Copy(const IComponent& component) override;

		virtual void Delete() override;

		virtual IComponent* New(GameObject* owner) override;
	public:

		static IComponent* Create(GameObject* owner);
		
		BoxCollider3D() = default;

		~BoxCollider3D() = default;

		void SetHalfExtent(const glm::vec3& halfExtent);

		glm::vec3 GetHalfExtent() const { return m_HalfExtent; }
	};

}
