#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"

#include <memory>

namespace Pengine
{

	class PENGINE_API PointLight : public IComponent
	{
	private:

		std::vector<glm::mat4> m_ShadowsTransforms;
		std::shared_ptr<class FrameBuffer> m_ShadowsCubeMap;

		/**
		 * This value is set up by the user and is the flag that indicates that this light source casts shadows.
		 */
		bool m_DrawShadows = false;
		
		/**
		 * This value will be calculated during the preparation stage as a flag that indicates
		 * that the PointLight is out of visible distance (Zfar) and shadows shouln't be rendered.  
		 */
		bool m_ShadowsVisible = false;

		friend class Renderer;
		friend class Scene;
	public:

		glm::vec3 m_Color = { 1.0f, 1.0f, 1.0f };

		float m_Constant = 1.0f;
		float m_Linear = 0.09f;
		float m_Quadratic = 0.032f;
		float m_ZFar = 150.0f;
		float m_ZNear = 0.1f;
		float m_Fog = 0.2f;

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

		void SetDrawShadows(bool drawShadows);

		bool IsDrawShadows() const { return m_DrawShadows; }

		bool IsRenderShadows() const;
	};

}