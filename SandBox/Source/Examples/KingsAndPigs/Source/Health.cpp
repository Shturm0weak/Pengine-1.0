#include "Health.h"

#include "Core/ComponentManager.h"
#include "Core/GameObject.h"
#include "UI/Gui.h"

using namespace Pengine;

void Health::TakeDamage(int damage)
{
	m_CurrentHealth = glm::clamp(m_CurrentHealth - damage, 0, m_MaxHealth);

	if (IsDead())
	{
		if (m_OnDeathCallback)
		{
			m_OnDeathCallback();
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
	COPY_PROPERTIES(health)
	m_Type = component.GetType();
}

IComponent* Health::Create(GameObject* owner)
{
	return new Health();
}

Health::Health()
{
	m_IdleAnimation = Animation2DManager::GetInstance().Load("Source\\Examples\\KingsAndPigs\\Animations\\SmallHeart\\Idle.anim");
	m_HitAnimation = Animation2DManager::GetInstance().Load("Source\\Examples\\KingsAndPigs\\Animations\\SmallHeart\\Hit.anim");

	TextureManager::GetInstance().Create("Source\\Examples\\KingsAndPigs\\Sprites\\12-Live and Coins\\Live Bar.png",
		[this](Texture* texture)
	{
		m_BarTexture = texture;
	});
}

void Health::DrawHearts(bool isHit)
{
	Gui& gui = Gui::GetInstance();
	const glm::vec2 halfSize = gui.m_ViewportUISize * 0.5f;
	const glm::vec2 barSize = { 356.0f, 200.0f };
	const glm::vec2 heartSize = { 130.0f, 101.0f };
	const glm::vec2 offset = { halfSize.x * 0.05f + 40.0f, -halfSize.y * 0.13f };
	const float speed = 8.0f;

	gui.Image({ -halfSize.x + halfSize.x * 0.05f, halfSize.y - halfSize.y * 0.05f },
		m_BarTexture, barSize);

	for (size_t i = 0; i < m_CurrentHealth; i++)
	{
		std::string label = "Heart" + std::to_string(i);
		if (i == m_CurrentHealth - 1)
		{
			if (isHit)
			{
				gui.Animation(label, { -halfSize.x + heartSize.x * 0.45f * i + offset.x, halfSize.y + offset.y },
					m_HitAnimation, heartSize, speed);
			}
			else
			{
				gui.Animation(label, { -halfSize.x + heartSize.x * 0.45f * i + offset.x, halfSize.y + offset.y },
					m_IdleAnimation, heartSize, speed);
			}
		}
		else
		{
			gui.Animation(label, { -halfSize.x + heartSize.x * 0.45f * i + offset.x, halfSize.y + offset.y },
				m_IdleAnimation, heartSize, speed);
		}
	}
}
