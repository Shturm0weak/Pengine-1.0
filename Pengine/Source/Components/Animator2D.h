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

		std::unordered_map<std::string, std::function<bool()>> m_EndCallbacksByName;
		std::vector<Animation2DManager::Animation2D*> m_Animations;

		Animation2DManager::Animation2D* m_CurrentAnimation = nullptr;
		
		float m_Timer = 0.0f;
		float m_FrameCounter = 0.0f;
		float m_Speed = 8.0f;
		bool m_Play = false;
		bool m_AutoSetUV = true;

	protected:

		virtual void OnStart() override;

		virtual void OnClose() override;

		virtual void OnUpdate() override;

		virtual void Copy(const IComponent& component) override;

		virtual void OnRegisterClient() override;

		virtual IComponent* New(GameObject* owner) override;
	public:

		static IComponent* Create(GameObject* owner);

		Animator2D() = default;
		
		~Animator2D() override;

		Animator2D(const Animator2D& a2d);

		Animator2D& operator=(const Animator2D& a2d);

		float GetSpeed() const { return m_Speed; }

		void SetSpeed(float speed) { m_Speed = speed; }

		bool IsPlaying() const { return m_Play; }
		
		void SetPlay(bool play) { m_Play = play; }
		
		void SetAutoUV(bool autoUV) { m_AutoSetUV = autoUV; }
		
		std::vector<Animation2DManager::Animation2D*> GetAnimations() const { return m_Animations; }

		Animation2DManager::Animation2D* GetCurrentAnimation() const { return m_CurrentAnimation; }
		
		Animation2DManager::Animation2D* GetAnimation(const std::string& name);
		
		void SetCurrentAnimation(Animation2DManager::Animation2D* animation) { m_CurrentAnimation = animation; }
		
		void PlayAnimation(Animation2DManager::Animation2D* animation);
		
		void AddAnimation(Animation2DManager::Animation2D* animation);
		
		void RemoveAnimation(Animation2DManager::Animation2D* animation);
		
		void RemoveAllAnimations();
		
		void AddEndCallback(const std::string& name, std::function<bool()> callback);

		std::function<bool()> GetEndCallback(const std::string& name);

		void RemoveEndCallback(const std::string& name);
		
		void RemoveAllEndCallbacks();

		void ResetTime();

		/**
		 * Returns the origin uv, otherwise an empty vector.
		 */
		std::vector<float> GetOriginalUV();
		
		/**
		 * Returns the reversed uv, otherwise an empty vector.
		 */
		std::vector<float> GetReversedUV();
	};

}
