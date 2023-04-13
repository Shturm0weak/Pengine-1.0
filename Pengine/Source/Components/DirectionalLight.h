#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"

namespace Pengine
{

	class PENGINE_API DirectionalLight : public IComponent
	{
	private:

		std::vector<float> m_CascadesDistance = { 5.0f, 15.0f };
		std::vector<float> m_Bias = { 0.001f, 0.001f, 0.001f };

		glm::vec3 m_Direction;
		glm::vec3 m_Color = { 1.0f, 1.0f, 1.0f };

		int m_Pcf = 2;
		float m_Texels = 1.0f;
		float m_ZFarScale = 0.1f;
		float m_Fog = 0.2f;
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

		int GetPcf() const { return m_Pcf; }

		void SetPcf(float pcf) { m_Pcf = pcf; }

		float GetTexels() const { return m_Texels; }

		void SetTexels(float texels) { m_Texels = texels; }

		std::vector<float> GetBias() const { return m_Bias; }

		void SetBias(std::vector<float> bias) { m_Bias = bias; }

		float GetZFarScale() const { return m_ZFarScale; }

		void SetZFarScale(float zFarScale) { m_ZFarScale = zFarScale; }

		float GetFog() const { return m_Fog; }

		void SetFog(float fog) { m_Fog = fog; }

		std::vector<float> GetCascadesDistance() const { return m_CascadesDistance; }

		void SetCascadesDistance(const std::vector<float>& cascadesDistance) { m_CascadesDistance = cascadesDistance;}
	};

}