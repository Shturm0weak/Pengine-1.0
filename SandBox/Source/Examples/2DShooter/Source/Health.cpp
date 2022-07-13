#include "Health.h"

#include "Pawn.h"

using namespace Pengine;

void Health::TakeDamage(float damage, Pawn* attacker)
{
	m_CurrentHealth = glm::clamp(m_CurrentHealth - damage, 0.0f, m_MaxHealth);

	if (IsDead())
	{
		if (m_OnDeathCallback)
		{
			m_OnDeathCallback(attacker);
		}
	}
}

IComponent* Health::New(GameObject* owner)
{
	return Create(owner);
}

void Health::Copy(const IComponent& component)
{
	Health& health = *(Health*)&component;
	m_Type = component.GetType();
}

IComponent* Health::Create(GameObject* owner)
{
	return new Health();
}
