#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"
#include "../Core/TextureManager.h"
#include "../EventSystem/Listener.h"

#include <random>

namespace Pengine
{

	class PENGINE_API ParticleEmitter : public IComponent, public IListener
	{
	private:

		enum class FacingMode
		{
			DEFAULT,
			SPHERICAL,
			CYLINDRICAL
		} m_FacingMode = FacingMode::DEFAULT;

		struct Particle
		{
			glm::vec4 m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };
			glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
			glm::vec3 m_Direction = { 0.0f, 0.0f, 0.0f };
			glm::vec3 m_Scale = { 0.0f, 0.0f, 0.0f };
			glm::vec3 m_Speed = { 0.0f, 0.0f, 0.0f };
			Texture* m_Texture = nullptr;
			float m_TimeLiving = 0.0f;
			float m_TimeToSpawn = 0.0f;
			bool m_IsEnabled = false;

			void Initialize(ParticleEmitter& particleEmitter, std::mt19937& engine);
		};

		glm::vec4 m_ColorStart = { 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec4 m_ColorEnd = { 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec3 m_Gravity = { 0.0f, 0.0f, 0.0f };
		glm::vec2 m_Scale = { 1.0f, 1.0f };
		glm::vec2 m_TimeToSpawn = { 0.0f, 1.0f };
		std::vector<glm::vec2> m_Direction = { { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f } };
		std::vector<glm::vec2> m_RadiusToSpawn = { { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f } };
		std::vector<Particle> m_Particles;
		Texture* m_Texture = TextureManager::GetInstance().White();
		int m_ParticlesSize = 10;
		int m_ParticlesFinished = 0;
		float m_MaxTimeToLive = 1.0f;
		float m_Acceleration = 1.0f;
		float m_Intensity = 1.0f;
		bool m_Loop = true;

		std::vector<float> m_MeshVertexAttributes;

		friend class Editor;
		friend class Serializer;
	protected:

		virtual void OnStart() override;

		virtual void OnUpdate() override;

		virtual void OnClose() override;

		virtual void Copy(const IComponent& component) override;

		virtual void Delete() override;

		virtual IComponent* New(GameObject* owner) override;
	public:

		std::vector<std::function<void()>> m_EndCallbacks;
		
		static IComponent* Create(GameObject* owner);

		ParticleEmitter();

		~ParticleEmitter();

		void Render();
	};

}