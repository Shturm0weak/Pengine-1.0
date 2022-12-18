#pragma once

#include "Core/Core.h"
#include "Core/Component.h"
#include "Core/Utils.h"
#include "Core/Animation2DManager.h"

#include <functional>

using namespace Pengine;

class Health : public IComponent
{
	REGISTER_PARENT_CLASS(IComponent)

private: PROPERTY(int, m_CurrentHealth, 3)
private: PROPERTY(int, m_MaxHealth, 3)

	std::function<void()> m_OnDeathCallback;

	Animation2DManager::Animation2D* m_IdleAnimation;
	Animation2DManager::Animation2D* m_HitAnimation;

	Texture* m_BarTexture = nullptr;
public:

	virtual class IComponent* New(class GameObject* owner) override;

	virtual void Copy(const IComponent& component) override;

	static class IComponent* Create(class GameObject* owner);

	Health();

	void DrawHearts(bool isHit);

	void TakeDamage(int damage);

	float GetMaxHealth() const { return m_MaxHealth; }

	float GetCurrentHealth() const { return m_CurrentHealth; }

	void SetCurrentHealth(int health) { m_CurrentHealth = glm::clamp(health, 0, m_MaxHealth); }

	void SetMaxHealth(int health) { m_MaxHealth = glm::clamp(health, 0, health); }

	void SetOnDeathCallback(std::function<void()> callback) { m_OnDeathCallback = callback; }

	bool IsDead() { return m_CurrentHealth <= Utils::Epsilonf(); }
};
REGISTER_COMPONENT(Health)