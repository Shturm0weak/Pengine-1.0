#pragma once

#include "Core/Core.h"
#include "Core/Component.h"
#include "Core/Utils.h"

#include <functional>

class Health : public Pengine::IComponent
{
	RTTR_ENABLE(IComponent)

private:

	private: PROPERTY(Health, float, m_CurrentHealth, 100.0f)
	private: PROPERTY(Health, float, m_MaxHealth, 100.0f)

	std::function<void(class Pawn*)> m_OnDeathCallback;
public:

	virtual class Pengine::IComponent* New(class Pengine::GameObject* owner) override;
	virtual void Copy(const Pengine::IComponent& component) override;

	void TakeDamage(float damage, class Pawn* attacker);

	float GetMaxHealth() const { return m_MaxHealth; }
	float GetCurrentHealth() const { return m_CurrentHealth; }
	void SetCurrentHealth(float health) { m_CurrentHealth = glm::clamp(health, 0.0f, m_MaxHealth); }
	void SetMaxHealth(float health) { m_MaxHealth = glm::clamp(health, 0.0f, health); }
	void SetOnDeathCallback(std::function<void(class Pawn*)> callback) { m_OnDeathCallback = callback; }
	bool IsDead() { return m_CurrentHealth < Pengine::Utils::Epsilonf(); }

	static class Pengine::IComponent* Create(class Pengine::GameObject* owner);
};
REGISTER_CLASS(Health)