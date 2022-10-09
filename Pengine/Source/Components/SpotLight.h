#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"
#include "../Core/Allocator.h"

namespace Pengine
{

	class PENGINE_API SpotLight : public IComponent, public IAllocator
	{
	public:

		glm::vec3 m_Color = { 1.0f, 1.0f, 1.0f };
	
		float m_Constant = 1.0f;
		float m_Linear = 0.09f;
		float m_Quadratic = 0.032f;
	private:

		glm::vec3 m_Direction;

		float m_InnerCutOff = glm::radians(12.5f);
		float m_OuterCutOff = glm::radians(17.5f);
		float m_CosInnerCutOff = glm::cos(m_InnerCutOff);
		float m_CosOuterCutOff = glm::cos(m_OuterCutOff);

		friend class Instancing;
	public:

		virtual IComponent* New(GameObject* owner) override;

		virtual void Copy(const IComponent& component) override;

		virtual void Delete() override;

		SpotLight() = default;
		~SpotLight() = default;

		static IComponent* Create(GameObject* owner);

		void SetColor(const glm::vec3& color) { m_Color = color; }

		void SetConstant(float constant) { m_Constant = constant; }

		void SetLinear(float linear) { m_Linear = linear; }

		void SetQuadratic(float quadratic) { m_Quadratic = quadratic; }

		void SetInnerCutOff(float innerCutOff);

		float GetInnerCutOff() const { return m_InnerCutOff; }

		void SetOuterCutOff(float outerCutOff);

		float GetOuterCutOfff() const { return m_OuterCutOff; }

		void SetDirection(const glm::vec3 direction) { m_Direction = direction; }

		glm::vec3 GetDirection() const { return m_Direction; }
	};

}