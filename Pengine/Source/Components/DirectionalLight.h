#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"

namespace Pengine
{

	class PENGINE_API DirectionalLight : public IComponent
	{
	public:

		glm::vec3 m_Color = { 1.0f, 1.0f, 1.0f };
		float m_Intensity = 1.0f;

		virtual IComponent* New(GameObject* owner) override;

		virtual void Copy(const IComponent& component) override;

		virtual void Delete() override;

		DirectionalLight() = default;
		~DirectionalLight() = default;

		static IComponent* Create(GameObject* owner);
	};

}