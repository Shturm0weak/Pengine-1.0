#pragma once

#include "Core/Core.h"
#include "Core/Component.h"
#include "Core/ReflectionSystem.h"
#include "EventSystem/Listener.h"
#include "Components/Rigidbody2D.h"
#include "Components/Renderer2D.h"
#include "Components/BoxCollider2D.h"
#include "Components/Animator2D.h"

class Controller : public Pengine::IComponent
{
	RTTR_ENABLE(IComponent)

private:

	void OnDirectionChanged();

	class Pawn* m_Pawn = nullptr;

	bool m_IsLanded = false;
	int m_PreviousDirection = 1;
	int m_Direction = 1;
	int m_State = 0;
	public: PROPERTY(Controller, float, m_Speed, 2.0f)
	public: PROPERTY(Controller, float, m_Jump, 3.0f)
public:

	virtual class Pengine::IComponent* New(class Pengine::GameObject* owner) override;
	virtual void Copy(const Pengine::IComponent& component) override;

	bool IsLandedCalculate();

	friend class Shooter2D;
public:

	void MoveRight();
	void MoveLeft();
	void Idle();
	void Jump();
	void Update();
	void SetSpeed(float speed) { m_Speed = glm::clamp(speed, 0.0f, speed); }
	void SetJumpForce(float jumpForce) { m_Jump = glm::clamp(jumpForce, 0.0f, jumpForce); }
	void SetPawn(class Pawn* pawn) { m_Pawn = pawn; }
	bool IsLanded() const { return m_IsLanded; }
	float GetSpeed() const { return m_Speed; }
	float GetJumpForce() const { return m_Jump; }
	int GetDirection() const { return m_Direction; }

	static class Pengine::IComponent* Create(class Pengine::GameObject* owner);
};
REGISTER_CLASS(Controller)