#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"

namespace Pengine
{

	class PENGINE_API DirectionalLight : public IComponent
	{
	private:

		glm::vec3 m_Direction;
		glm::vec3 m_Color = { 1.0f, 1.0f, 1.0f };

		float m_Intensity = 1.0f;
	protected:

		virtual void Copy(const IComponent& component) override;

		virtual void Delete() override;

		virtual IComponent* New(GameObject* owner) override;
	public:

		static IComponent* Create(GameObject* owner);

		DirectionalLight() = default;

		~DirectionalLight() = default;

		void SetDirection(const glm::vec3 direction) { m_Direction = direction; }

		glm::vec3 GetDirection() const { return m_Direction; }

		void SetColor(const glm::vec3& color) { m_Color = color; }

		glm::vec3 GetColor() const { return m_Color; }

		void SetIntensity(float intensity) { m_Intensity = intensity; }

		float GetIntensity() const { return m_Intensity; }
	};

}