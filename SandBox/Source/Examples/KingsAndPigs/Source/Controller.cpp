#include "Controller.h"

#include "Core/GameObject.h"
#include "Core/Raycast2D.h"
#include "Core/Scene.h"
#include "Core/Visualizer.h"
#include "Core/Timer.h"
#include "Core/EntryPoint.h"
#include "Core/Input.h"
#include "EventSystem/EventSystem.h"
#include "Events/OnMainThreadCallback.h"

IComponent* Controller::New(GameObject* owner)
{
	return Create(owner);
}

void Controller::Copy(const IComponent& component)
{
	COPY_PROPERTIES(Controller, component)

	Controller& controller = *(Controller*)&component;
	m_Type = component.GetType();
}

Controller::Controller()
{
	std::string directory = "Source/Examples/KingsAndPigs/Sounds/";

	//m_HitSounds.push_back(SoundManager::GetInstance().Load(directory + "Hit1.wav"));
	m_HitSounds.push_back(SoundManager::GetInstance().Load(directory + "Hit2.wav"));
	m_HitSounds.push_back(SoundManager::GetInstance().Load(directory + "Hit3.wav"));

	//m_JumpSounds.push_back(SoundManager::GetInstance().Load(directory + "Jump1.wav"));
	//m_JumpSounds.push_back(SoundManager::GetInstance().Load(directory + "Jump2.wav"));
	m_JumpSounds.push_back(SoundManager::GetInstance().Load(directory + "Jump3.wav"));

	m_AttackSound = SoundManager::GetInstance().Load(directory + "Attack.wav");

	auto onStartCallback = [this]
	{
		m_A2d = GetOwner()->m_ComponentManager.AddComponent<Animator2D>();
		m_R2d = GetOwner()->m_ComponentManager.AddComponent<Renderer2D>();
		m_Bc2d = GetOwner()->m_ComponentManager.AddComponent<BoxCollider2D>();
		m_Health = GetOwner()->m_ComponentManager.AddComponent<Health>();

		auto callback = [this]
		{
			if (m_HasRigidbody)
			{
				m_Rb2d = GetOwner()->m_ComponentManager.AddComponent<Rigidbody2D>();
			}
		};
		EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));

		auto deathCallback = [this]
		{
			auto callback1 = [this]
			{
				m_IsDead = true;
				m_Bc2d->m_Tag = "Dead";
				m_A2d->Reset();

				m_A2d->m_EndCallbacks.push_back([this]
					{
						auto callback2 = [this]
						{
							m_A2d->SetPlay(false);
							GetOwner()->DeleteLater(m_DeathSpeed);
						};
						EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback2, EventType::ONMAINTHREADPROCESS));

						return true;
					}
				);
			};
			EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback1, EventType::ONMAINTHREADPROCESS));
		};

		m_Health->SetOnDeathCallback(deathCallback);
	};
	EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(onStartCallback, EventType::ONMAINTHREADPROCESS));
}

Controller::~Controller()
{
	EventSystem::GetInstance().UnregisterAll(this);
}

void Controller::UpdateAnimation()
{
	std::vector<float> uv;
	if (m_Direction == 1)
	{
		uv = m_UVReversedByDefault ? m_A2d->GetReversedUV() : m_A2d->GetOriginalUV();
	}
	else if(m_Direction == -1)
	{
		uv = m_UVReversedByDefault ? m_A2d->GetOriginalUV() : m_A2d->GetReversedUV();
	}

	Animation2DManager::Animation2D* animation;

	if (m_IsDead)
	{
		animation = m_A2d->GetAnimation("Death");
	}
	else if (m_IsHit)
	{
		animation = m_A2d->GetAnimation("Hit");
	}
	else if (m_IsAttacking)
	{
		animation = m_A2d->GetAnimation("Attack");
	}
	else if (m_IsLanded)
	{
		if (m_IsIdle)
		{
			animation = m_A2d->GetAnimation("Idle");
		}
		else
		{
			animation = m_A2d->GetAnimation("Run");
		}
	}
	else
	{
		if (m_IsFalling)
		{
			animation = m_A2d->GetAnimation("Fall");
		}
		else
		{
			animation = m_A2d->GetAnimation("Jump");
		}
	}
	
	m_R2d->SetUV(uv);
	m_A2d->SetCurrentAnimation(animation);
}

void Controller::UpdateParams()
{
	if (m_Rb2d)
	{
		m_Rb2d->SetAllowToSleep(false);
	}

	m_IsLanded = IsLandedCalculate();

	m_IsIdle = false;

	m_IsFalling = (GetOwner()->m_Transform.GetPreviousPosition() -
		GetOwner()->m_Transform.GetPosition()).y < 0.0f;

	m_AttackCoolDownTimer -= Time::GetDeltaTime();
}

void Controller::Hit(float damage)
{
	m_IsHit = true;

	if (m_IsAttacking)
	{
		m_A2d->m_EndCallbacks[m_AttackIndexCallback]();
		m_A2d->m_EndCallbacks.erase(m_A2d->m_EndCallbacks.begin() + m_AttackIndexCallback);
	}

	m_A2d->Reset();
	m_A2d->m_EndCallbacks.push_back([damage, this] {
		m_IsHit = false;

		m_Health->TakeDamage(damage);

		return true;
	});

	std::random_device random;
	std::mt19937 engine(random());

	std::uniform_int_distribution<int> hitSound(0, m_HitSounds.size() - 1);

	SoundManager::GetInstance().Play(m_HitSounds[hitSound(engine)]);
}

IComponent* Controller::Create(GameObject* owner)
{
	return new Controller();
}

void Controller::OnRegisterClient()
{
	EventSystem::GetInstance().RegisterClient(EventType::ONSTART, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONCLOSE, this);
}

void Controller::OnUpdate()
{
	UpdateParams();
	Movement();
	UpdateAnimation();
}

bool Controller::IsLandedCalculate()
{
	glm::vec3 position = GetOwner()->m_Transform.GetPosition() + glm::vec3(m_Bc2d->GetOffset(), 0.0f);
	glm::vec2 size = m_Bc2d->GetSize() * glm::vec2(GetOwner()->m_Transform.GetScale());
	float length = size.y * 1.1f;

	Raycast2D::Hit2D hits[3];

	Raycast2D::Raycast(m_Owner->GetScene(), glm::vec2(position) - glm::vec2(size.x, 0.0f),
		glm::vec2(position) - glm::vec2(size.x, 0.0f) + glm::vec2(-0.1f, -length), hits[0], m_IgnoreGroundColliderTags);

	Raycast2D::Raycast(m_Owner->GetScene(), glm::vec2(position),
		glm::vec2(position) + glm::vec2(0.0f, -length), hits[1], m_IgnoreGroundColliderTags);
	
	Raycast2D::Raycast(m_Owner->GetScene(), glm::vec2(position) + glm::vec2(size.x, 0.0f),
		glm::vec2(position) + glm::vec2(size.x, 0.0f) + glm::vec2(0.1f, -length), hits[2], m_IgnoreGroundColliderTags);

	//Visualizer::DrawLine(position + glm::vec3(size.x, 0.0f, 0.0f), position + glm::vec3(size.x, 0.0f, 0.0f) + glm::vec3(0.0f, -1000.0f, 0.0f), { 0.0f, 0.0f, 1.0f, 1.0f });

	for (auto hit : hits)
	{
		if (hit.GetCollider())
		{
			float distance = glm::abs(hit.GetPosition().y - position.y);
			if (hit.GetCollider()->m_Tag == m_GroundColliderTag)
			{
				//Visualizer::DrawLine(position, { hit.GetPosition(), 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });

				return true;
			}
		}
	}

	return false;
}

void Controller::MoveRight()
{
	if (m_Direction == -1)
	{
		m_Bc2d->m_Offset.x = -m_Bc2d->m_Offset.x;
	}

	m_Owner->m_Transform.Translate(m_Owner->m_Transform.GetPosition()
		+ glm::vec3(m_Speed * Time::GetDeltaTime(), 0.0f, 0.0f));
	m_Direction = 1;
}

void Controller::MoveLeft()
{
	if (m_Direction == 1)
	{
		m_Bc2d->m_Offset.x = -m_Bc2d->m_Offset.x;
	}

	m_Owner->m_Transform.Translate(m_Owner->m_Transform.GetPosition()
		- glm::vec3(m_Speed * Time::GetDeltaTime(), 0.0f, 0.0f));
	m_Direction = -1;
}

void Controller::Idle()
{
	m_IsIdle = true;
}

void Controller::Jump()
{
	if (m_IsLanded)
	{
		m_IsLanded = false;
		if (m_Rb2d->GetBody())
		{
			m_Rb2d->GetBody()->ApplyLinearImpulseToCenter({ 0.0f, m_Jump }, true);
		}

		std::random_device random;
		std::mt19937 engine(random());

		std::uniform_int_distribution<int> jumpSound(0, m_JumpSounds.size() - 1);

		SoundManager::GetInstance().Play(m_JumpSounds[jumpSound(engine)]);
	}
}

void Controller::AttackStart()
{
	m_AttackCoolDownTimer = m_AttackCoolDown;
	m_IsAttacking = true;

	SoundManager::GetInstance().Play(m_AttackSound);
}

void Controller::AttackEnd(std::function<void()> callback)
{
	m_A2d->Reset();
	m_AttackIndexCallback = m_A2d->m_EndCallbacks.size();
	m_A2d->m_EndCallbacks.push_back([this, callback]
		{
			m_IsAttacking = false;

			if (callback)
			{
				callback();
			}

			return true;
		}
	);
}

void Controller::Attack()
{
	if (!m_IsAttacking && !m_IsHit && m_AttackCoolDownTimer <= 0.0f)
	{
		AttackStart();
	
		glm::vec2 start = m_Bc2d->GetPosition();
		glm::vec2 end = start + glm::vec2(m_Direction * m_AttackRange, 0.0f);
		Raycast2D::Hit2D hit;
		Raycast2D::Raycast(GetOwner()->GetScene(), start, end, hit, m_IgnoreAttackColliderTags);

		if (hit.GetCollider() && hit.GetCollider()->m_Tag == m_EnemyColliderTag)
		{
			GameObject* enemy = hit.GetCollider()->GetOwner();
			if (Rigidbody2D* rb2d = enemy->m_ComponentManager.GetComponent<Rigidbody2D>())
			{
				rb2d->ApplyLinearImpulseToCenter(glm::vec2(m_Direction * m_AttackImpulse.x,
					m_AttackImpulse.y), true);
			}

			if (Controller* controller = enemy->m_ComponentManager.GetComponentSubClass<Controller>())
			{
				controller->m_LastHitInstigator = GetOwner();
				controller->Hit(m_Damage);
			}
		}

		AttackEnd({});
	}
}

void Controller::Movement()
{
	
}
