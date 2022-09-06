#pragma once

#include "Core/Core.h"
#include "Core/Component.h"
#include "Core/ReflectionSystem.h"
#include "EventSystem/Listener.h"
#include "Components/Rigidbody2D.h"
#include "Components/Renderer2D.h"
#include "Components/BoxCollider2D.h"
#include "Components/Animator2D.h"
#include "Audio/SoundManager.h"

#include "Door.h"
#include "Health.h"

using namespace Pengine;

class Controller : public IComponent, public IListener
{
	RTTR_ENABLE(IComponent)

private:

	bool IsLandedCalculate();

public: PROPERTY(Controller, bool, m_UVReversedByDefault, false)
public: PROPERTY(Controller, bool, m_HasRigidbody, true)
public: PROPERTY(Controller, int, m_Direction, 1)
public: PROPERTY(Controller, int, m_Damage, 1)
public: PROPERTY(Controller, float, m_DeathSpeed, 0.3f)
public: PROPERTY(Controller, float, m_Speed, 2.0f)
public: PROPERTY(Controller, float, m_Jump, 3.0f)
public: PROPERTY(Controller, float, m_AttackRange, 0.5f)
public: PROPERTY(Controller, float, m_AttackCoolDown, 1.0f)
public: PROPERTY(Controller, glm::vec2, m_AttackImpulse, { 0.5f COM 0.25f })
public: PROPERTY(Controller, std::string, m_GroundColliderTag, "")
public: PROPERTY(Controller, std::string, m_EnemyColliderTag, "")
public: PROPERTY(Controller, std::vector<std::string>, m_IgnoreGroundColliderTags, {})
public: PROPERTY(Controller, std::vector<std::string>, m_IgnoreAttackColliderTags, {})

	std::vector<Sound*> m_JumpSounds;
	std::vector<Sound*> m_HitSounds;

	Sound* m_AttackSound;

	Animator2D* m_A2d = nullptr;
	Renderer2D* m_R2d = nullptr;
	BoxCollider2D* m_Bc2d = nullptr;
	Rigidbody2D* m_Rb2d = nullptr;
	Health* m_Health = nullptr;

	GameObject* m_LastHitInstigator = nullptr;

	size_t m_AttackIndexCallback = 0;
	int m_PreviousDirection = 0;
	float m_AttackCoolDownTimer = 0.0f;
	bool m_IsLanded = false;
	bool m_IsIdle = false;
	bool m_IsFalling = false;
	bool m_IsAttacking = false;
	bool m_IsHit = false;
	bool m_IsDead = false;
protected:

	void AttackStart();

	void AttackEnd(std::function<void()> callback);

	virtual void Attack();

	virtual void Movement();

	virtual void UpdateAnimation();

	void UpdateParams();
public:

	virtual class IComponent* New(class GameObject* owner) override;

	virtual void Copy(const IComponent& component) override;

	Controller();
	virtual ~Controller();

	static class IComponent* Create(class Pengine::GameObject* owner);

	virtual void OnRegisterClient() override;

	virtual void OnUpdate() override;

	BoxCollider2D* GetBc2d() const { return m_Bc2d; }

	Animator2D* GetA2d() const { return m_A2d; }

	Rigidbody2D* GetRb2d() const { return m_Rb2d; }

	Renderer2D* GetR2d() const { return m_R2d; }

	Health* GetHealth() const { return m_Health; }

	GameObject* GetLastHitInstigator() const { return m_LastHitInstigator; }

	void SetLastHitInstigator(GameObject* lastHitInstigator) { m_LastHitInstigator = lastHitInstigator; }

	bool IsAbleToAttack() const { return m_AttackCoolDownTimer < 0.0f; }

	bool IsDead() const { return m_IsDead; }

	bool IsHit() const { return m_IsHit; }

	bool IsFalling() const { return m_IsFalling; }

	bool IsAttacking() const { return m_IsAttacking; }

	bool IsIdle() const { return m_IsIdle; }

	void MoveRight();

	void MoveLeft();

	void Idle();

	void Jump();

	void Hit(float damage);

	void SetSpeed(float speed) { m_Speed = glm::clamp(speed, 0.0f, speed); }

	void SetJumpForce(float jumpForce) { m_Jump = glm::clamp(jumpForce, 0.0f, jumpForce); }

	bool IsLanded() const { return m_IsLanded; }

	float GetSpeed() const { return m_Speed; }

	float GetJumpForce() const { return m_Jump; }

	int GetDirection() const { return m_Direction; }
};
REGISTER_CLASS(Controller)