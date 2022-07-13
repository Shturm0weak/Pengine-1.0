#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"
#include "../Core/Animation2DManager.h"
#include "../EventSystem/Listener.h"

#include <vector>
#include <unordered_map>

namespace Pengine
{

	class PENGINE_API Animator2D : public IComponent, public IListener
	{
	private:

		const char** m_AnimationsNames = nullptr;
		const char** GetAnimationsNames();

		class Renderer2D* m_Renderer2D = nullptr;
		Animation2DManager::Animation2D* m_CurrentAnimation = nullptr;

		float m_Timer = 0.0f;
		float m_Counter = 0.0f;
		float m_Speed = 8.0f;
		bool m_Play = false;
		bool m_AutoSetUV = true;

		friend class Editor;
	public:

		std::vector<std::function<void()>> m_EndCallbacks;
		std::vector<Animation2DManager::Animation2D*> m_Animations;

		static IComponent* Create(GameObject* owner);

		Animator2D();

		virtual ~Animator2D() override;

		virtual void OnStart() override;
		
		virtual void OnClose() override;
		
		virtual void OnUpdate() override;
		
		virtual void Copy(const IComponent& component) override;
		
		virtual IComponent* New(GameObject* owner) override;

		float GetSpeed() const { return m_Speed; }

		void SetSpeed(float speed) { m_Speed = speed; }

		bool IsPlaying() const { return m_Play; }
		
		void SetPlay(bool play) { m_Play = play; }
		
		void SetAutoUV(bool autoUV) { m_AutoSetUV = autoUV; }
		
		Animation2DManager::Animation2D* GetCurrentAnimation() const { return m_CurrentAnimation; }
		
		Animation2DManager::Animation2D* GetAnimation(const std::string& name);
		
		void SetCurrentAnimation(Animation2DManager::Animation2D* animation) { m_CurrentAnimation = animation; }
		
		void PlayAnimation(Animation2DManager::Animation2D* animation);
		
		void AddAnimation(Animation2DManager::Animation2D* animation);
		
		void RemoveAnimation(Animation2DManager::Animation2D* animation);
		
		void RemoveAll();
		
		void Reset();

		std::vector<float> GetOriginalUV();
		
		std::vector<float> GetReversedUV();
	};

}
