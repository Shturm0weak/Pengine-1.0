#pragma once

#include "Core/Core.h"
#include "Core/Component.h"
#include "Core/ReflectionSystem.h"
#include "EventSystem/Listener.h"
#include "Components/Rigidbody2D.h"
#include "Components/Renderer2D.h"
#include "Components/BoxCollider2D.h"
#include "Components/Animator2D.h"

class EnemyAI : public Pengine::IComponent, public Pengine::IListener
{
	RTTR_ENABLE(IComponent)

private:

	enum class STATE
	{
		PATROL,
		FIGHT
	};

	class Pengine::GameObject* m_StartPosition = nullptr;
	class Pengine::GameObject* m_EndPosition = nullptr;
	class Pengine::GameObject* m_PreviousTargetPosition = nullptr;
	class Pengine::GameObject* m_CurrentTargetPosition = nullptr;
	class Controller* m_Controller = nullptr;
	class Shoot* m_Shoot = nullptr;

	public: PROPERTY(EnemyAI, float, m_WaitTimer, 0.0f);
	public: PROPERTY(EnemyAI, float, m_TimeToWait, 3.0f);
	public: PROPERTY(EnemyAI, float, m_VisionLength, 3.0f);

	STATE m_State = STATE::PATROL;

	void Patrol();
	void Vision();
	void Fight();

	virtual Pengine::IComponent* New(class Pengine::GameObject* owner) override;
	virtual void Copy(const Pengine::IComponent& component) override;

	friend class Enemy;
public:

	virtual void OnStart() override;
	virtual void OnUpdate() override;
	virtual void OnClose() override;

	EnemyAI();
	~EnemyAI();

	static Pengine::IComponent* Create(class Pengine::GameObject* owner);
};
REGISTER_CLASS(EnemyAI)