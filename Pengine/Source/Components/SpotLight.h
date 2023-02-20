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

		float m_ZFar = 150.0f;
		float m_ZNear = 0.1f;
		float m_Fog = 0.2f;

		/**
		 * This value is set up by the user and is the flag that indicates that this light source casts shadows.
		 */
		bool m_DrawShadows = false;

		/**
		 * This value will be calculated during the preparation stage as a flag that indicates
		 * that the PointLight is out of visible distance (Zfar) and shadows shouln't be rendered.
		 */
		bool m_ShadowsVisible = false;
	private:

		glm::vec3 m_Direction;

		glm::mat4 m_ShadowTransform;
		std::shared_ptr<class FrameBuffer> m_ShadowMap;

		float m_InnerCutOff = glm::radians(12.5f);
		float m_OuterCutOff = glm::radians(17.5f);
		float m_CosInnerCutOff = glm::cos(m_InnerCutOff);
		float m_CosOuterCutOff = glm::cos(m_OuterCutOff);

		float m_ShadowBias = 0.0001f;
		int m_ShadowPcf = 1;

		friend class Instancing;
		friend class Renderer;
		friend class Renderer3D;
	protected:

		virtual void Copy(const IComponent& component) override;

		virtual void Delete() override;

		virtual IComponent* New(GameObject* owner) override;
	public:

		static IComponent* Create(GameObject* owner);

		SpotLight() = default;

		~SpotLight() = default;

		void SetShadowBias(float shadowBias) { m_ShadowBias = shadowBias; }

		float GetShadowBias() const { return m_ShadowBias; }

		void SetShadowPcf(int shadowPcf) { m_ShadowPcf = shadowPcf; }

		int GetShadowPcf() const { return m_ShadowPcf; }

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

		void SetDrawShadows(bool drawShadows);

		bool IsDrawShadows() const { return m_DrawShadows; }

		bool IsRenderShadows() const;

		void BuildProjectionMatrix();
	};

}