#include "Enemy.h"

#include "EventSystem/EventSystem.h"
#include "Core/ComponentManager.h"

#include "Health.h"
#include "Controller.h"
#include "Shoot.h"
#include "EnemyAI.h"

using namespace Pengine;

Enemy::Enemy()
{
	EventSystem::GetInstance().RegisterClient(EventType::ONSTART, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONCLOSE, this);
}

Enemy::~Enemy()
{
	EventSystem::GetInstance().UnregisterAll(this);
}

void Enemy::OnStart()
{
	m_A2d = m_Owner->m_ComponentManager.GetComponent<Animator2D>();
	m_A2d->SetAutoUV(true);

	m_R2d = m_Owner->m_ComponentManager.GetComponent<Renderer2D>();
	m_Rb2d = m_Owner->m_ComponentManager.GetComponent<Rigidbody2D>();
	m_Bc2d = m_Owner->m_ComponentManager.GetComponent<BoxCollider2D>();
	m_Health = m_Owner->m_ComponentManager.AddComponent<Health>();

	m_Shoot = m_Owner->m_ComponentManager.AddComponent<Shoot>();
	m_Shoot->m_Pawn = this;
	m_Shoot->m_IgnoreMask.push_back("Enemy");
	m_Shoot->m_TargetTag = "Player";
	m_Shoot->m_FireRate = 200.0f;

	m_Controller = m_Owner->m_ComponentManager.AddComponent<Controller>();
	m_Controller->SetPawn(this);
	m_Controller->SetSpeed(1.5f);

	m_MuzzleFlash.m_GameObject = m_Owner->GetChilds()[0];
	m_MuzzleFlash.m_RelatedPosition = m_MuzzleFlash.m_GameObject->m_Transform.GetPosition() - m_Owner->m_Transform.GetPosition();
	m_MuzzleFlash.m_Renderer2D = m_MuzzleFlash.m_GameObject->m_ComponentManager.GetComponent<Renderer2D>();

	m_EnemyAI = m_Owner->m_ComponentManager.AddComponent<EnemyAI>();
	m_EnemyAI->m_Controller = m_Controller;
	m_EnemyAI->m_Shoot = m_Shoot;
}

void Enemy::OnUpdate()
{
	m_Controller->Update();

	if (m_Rb2d)
	{
		if (m_Rb2d->GetBody())
		{
			m_Rb2d->GetBody()->SetAwake(true);
		}
	}
}

void Enemy::OnClose()
{
}

IComponent* Enemy::Create(GameObject* owner)
{
	return new Enemy();
}

Pengine::IComponent* Enemy::New(Pengine::GameObject* owner)
{
	return Create(owner);
}
