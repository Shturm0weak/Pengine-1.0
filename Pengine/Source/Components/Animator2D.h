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

		float m_Timer = 0.0f;
		float m_Counter = 0.0f;

		const char** m_AnimationsNames = nullptr;
		const char** GetAnimationsNames();

		friend class Editor;
	public:

		std::vector<Animation2DManager::Animation2D*> m_Animations;
		Animation2DManager::Animation2D* m_CurrentAnimation = nullptr;
		float m_Speed = 8.0f;
		bool m_Play = false;

		static IComponent* Create(GameObject* owner);

		Animator2D();

		virtual ~Animator2D() override;
		virtual void OnStart() override;
		virtual void OnClose() override;
		virtual void OnUpdate() override;
		virtual void Copy(const IComponent& component) override;
		virtual IComponent* CreateCopy(GameObject* newOwner) override;

		void PlayAnimation(Animation2DManager::Animation2D* animation);
		void AddAnimation(Animation2DManager::Animation2D* animation);
		void RemoveAnimation(Animation2DManager::Animation2D* animation);
		void RemoveAll();
		void operator=(const Animator2D& animator2d);
	};

}
