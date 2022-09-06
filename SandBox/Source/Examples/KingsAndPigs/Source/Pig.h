#pragma once

#include "Core/Core.h"
#include "Core/Component.h"
#include "EventSystem/Listener.h"
#include "Components/Renderer2D.h"
#include "Components/Animator2D.h"
#include "Components/BoxCollider2D.h"
#include "Components/Rigidbody2D.h"

#include "Controller.h"

using namespace Pengine;

class Pig : public Controller
{
	RTTR_ENABLE(Controller)

private:

public: PROPERTY(Pig, int, m_InitializeDirection, 1)
public: PROPERTY(Pig, float, m_VisibilityRange, 2.0f)
public: PROPERTY(Pig, float, m_WaitTimer, 0.0f);
public: PROPERTY(Pig, float, m_TimeToWait, 3.0f);

	GameObject* m_StartPosition = nullptr;
	GameObject* m_EndPosition = nullptr;
	GameObject* m_PreviousTargetPosition = nullptr;
	GameObject* m_CurrentTargetPosition = nullptr;

	GameObject* m_King = nullptr;
protected:

	enum class STATE
	{
		PATROL,
		ATTACK,
		CHASE
	};

	STATE m_State = STATE::PATROL;

	virtual void LookForEnemy();

	virtual void Patrol();

	virtual void Chase();

	virtual void Attack() override;

	virtual void Movement() override;
public:

	virtual IComponent* New(GameObject* owner) override;

	virtual void Copy(const IComponent& component) override;

	static IComponent* Create(GameObject* owner);

	virtual void OnStart() override;
};
REGISTER_CLASS(Pig)