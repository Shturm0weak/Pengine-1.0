#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"

namespace Pengine
{

	class PENGINE_API PointLight : public IComponent
	{
	private:

		std::vector<glm::mat4> m_ShadowsTransforms;
		class FrameBuffer* m_ShadowsCubeMap = nullptr;

		friend class Renderer;
	public:

		glm::vec3 m_Color = { 1.0f, 1.0f, 1.0f };

		float m_Constant = 1.0f;
		float m_Linear = 0.09f;
		float m_Quadratic = 0.032f;
		float m_ZFar = 150.0f;
		float m_ZNear = 0.1f;
		float m_Fog = 0.2f;
		bool m_DrawShadows = true;

		virtual IComponent* New(GameObject* owner) override;

		virtual void Copy(const IComponent& component) override;

		virtual void Delete() override;

		PointLight() = default;
		~PointLight() = default;

		static IComponent* Create(GameObject* owner);

		void SetColor(const glm::vec3& color) { m_Color = color; }

		void SetConstant(float constant) { m_Constant = constant; }

		void SetLinear(float linear) { m_Linear = linear; }

		void SetQuadratic(float quadratic) { m_Quadratic = quadratic; }
	};

}