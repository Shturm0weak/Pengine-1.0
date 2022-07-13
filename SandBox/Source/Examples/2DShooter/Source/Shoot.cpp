#include "Shoot.h"

#include "Core/GameObject.h"
#include "Core/Raycast2D.h"
#include "Controller.h"
#include "EventSystem/EventSystem.h"
#include "Events/OnMainThreadCallback.h"
#include "Core/Timer.h"
#include "Core/Scene.h"
#include "Core/Serializer.h"
#include "Audio/SoundManager.h"

#include "Health.h"
#include "Player.h"

using namespace Pengine;

void Shoot::SpawnBloodEffect(const glm::vec3& position)
{
	GameObject* blood = m_Owner->GetScene()->CreateGameObject();
	blood->Copy(*m_BloodPrefab);
	blood->m_Transform.Translate(position);
	blood->m_IsEnabled = true;
	blood->m_IsSelectable = true;
	ParticleEmitter* bloodParticles = blood->m_ComponentManager.GetComponent<ParticleEmitter>();
	bloodParticles->OnStart();
	bloodParticles->m_EndCallbacks.push_back([=] {
		m_Owner->GetScene()->DeleteGameObject(blood);
	});
}

void Shoot::SpawnStonesEffect(const glm::vec3& position)
{
	GameObject* stone = m_Owner->GetScene()->CreateGameObject();
	stone->Copy(*m_StonesPrefab);
	stone->m_Transform.Translate(position);
	stone->m_IsEnabled = true;
	stone->m_IsSelectable = true;
	ParticleEmitter* stoneParticles = stone->m_ComponentManager.GetComponent<ParticleEmitter>();
	stoneParticles->OnStart();
	stoneParticles->m_EndCallbacks.push_back([=] {
		m_Owner->GetScene()->DeleteGameObject(stone);
	});
}

void Shoot::MuzzleFlash()
{
	m_Pawn->m_MuzzleFlash.m_GameObject->m_IsEnabled = true;
	Timer::SetCallback([=] {
		m_Pawn->m_MuzzleFlash.m_GameObject->m_IsEnabled = false;
	}, m_MuzzleFlashDuration);
}

IComponent* Shoot::New(GameObject* owner)
{
	return Create(owner);
}

void Shoot::Copy(const IComponent& component)
{
	Shoot& shoot = *(Shoot*)&component;
	m_Type = component.GetType();
}

void Shoot::OnUpdate()
{
	m_Timer -= Time::GetDeltaTime();
}

Shoot::Shoot()
{
	EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
	m_FireSound = SoundManager::GetInstance().Load("Source/Examples/2DShooter/Sounds/Fire.wav");
}

Shoot::~Shoot()
{
	EventSystem::GetInstance().UnregisterAll(this);
}

IComponent* Shoot::Create(GameObject* owner)
{
	return new Shoot();
}

void Shoot::Fire()
{
	if (m_Ammo == 0 || m_Timer > 0.0f) return;
	float millisecondsForShot = 1.0f / (m_FireRate / 60.0f);
	if (m_Pawn)
	{
		m_Ammo = glm::clamp(m_Ammo - 1, 0, 100000);
		glm::vec3 position = m_Pawn->m_MuzzleFlash.m_GameObject->m_Transform.GetPosition();
		Raycast2D::Hit2D hit;
		Raycast2D::Raycast(m_Owner->GetScene(), position, position + (float)m_Pawn->m_Controller->GetDirection() * glm::vec3(100.0f, 0.0f, 0.0f), hit, m_IgnoreMask);
		
		if (hit.GetCollider() && hit.GetCollider()->m_Tag == m_TargetTag)
		{
			SpawnBloodEffect({ hit.GetPosition(), 0.0f });

			GameObject* enemy = hit.GetCollider()->GetOwner();
			Health* health = enemy->m_ComponentManager.GetComponent<Health>();
			health->TakeDamage(m_Damage, m_Pawn);
		}
		else if (hit.GetCollider() && hit.GetCollider()->m_Tag == "Ground")
		{
			SpawnStonesEffect({ hit.GetPosition(), 0.0f });
		}

		m_Timer = millisecondsForShot;
		MuzzleFlash();
		SoundManager::GetInstance().Play(m_FireSound);
	}
}
