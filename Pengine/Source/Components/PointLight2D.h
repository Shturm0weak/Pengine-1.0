#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"

namespace Pengine
{

	class PointLight2D : public IComponent
	{
	public:

		glm::vec3 m_Color = { 1.0f,1.0f,1.0f };

		float m_Constant = 1.0f;
		float m_Linear = 0.09f;
		float m_Quadratic = 0.032f;


		virtual IComponent* New(GameObject* owner) override;

		virtual void Copy(const IComponent& component) override;

		virtual void Delete() override;

		PointLight2D() = default;
		~PointLight2D() = default;

		static IComponent* Create(GameObject* owner);

		void SetColor(const glm::vec3& color) { m_Color = color; }

		void SetConstant(float constant) { m_Constant = constant; }

		void SetLinear(float linear) { m_Linear = linear; }

		void SetQuadratic(float quadratic) { m_Quadratic = quadratic; }
	};

}