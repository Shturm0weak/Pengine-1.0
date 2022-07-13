#include "Player.h"

#include "EventSystem/EventSystem.h"
#include "Core/ComponentManager.h"
#include "Core/Input.h"
#include "Core/Environment.h"
#include "Core/Scene.h"
#include "Audio/SoundManager.h"

#include "Health.h"
#include "Controller.h"
#include "Shoot.h"

using namespace Pengine;

void Player::Movement()
{
	m_Controller->Update();
	
	if (Input::KeyBoard::IsKeyPressed(Keycode::SPACE))
	{
		m_Controller->Jump();
	}

	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_D))
	{
		m_Controller->MoveRight();
	}
	else if (Input::KeyBoard::IsKeyDown(Keycode::KEY_A))
	{
		m_Controller->MoveLeft();
	}
	else
	{
		m_Controller->Idle();
	}

	if (Input::Mouse::IsMouseDown(Keycode::MOUSE_BUTTON_1))
	{
		m_Shoot->Fire();
	}
}

Player::Player()
{
	m_PickUpSound = SoundManager::GetInstance().Load("Source/Examples/2DShooter/Sounds/PickUp.wav");

	EventSystem::GetInstance().RegisterClient(EventType::ONSTART, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONCLOSE, this);
}

Player::~Player()
{
	EventSystem::GetInstance().UnregisterAll(this);
}

void Player::OnStart()
{
	m_A2d = m_Owner->m_ComponentManager.GetComponent<Animator2D>();
	m_A2d->SetAutoUV(false);

	m_R2d = m_Owner->m_ComponentManager.GetComponent<Renderer2D>();
	m_Bc2d = m_Owner->m_ComponentManager.GetComponent<BoxCollider2D>();

	m_Rb2d = m_Owner->m_ComponentManager.GetComponent<Rigidbody2D>();
	if (m_Rb2d->GetBody())
	{
		m_Rb2d->GetBody()->SetSleepingAllowed(false);
	}

	m_Health = m_Owner->m_ComponentManager.AddComponent<Health>();
	m_Health->SetMaxHealth(200.0f);
	m_Health->SetCurrentHealth(m_Health->GetMaxHealth());

	m_Shoot = m_Owner->m_ComponentManager.AddComponent<Shoot>();
	m_Shoot->m_Pawn = this;
	m_Shoot->m_IgnoreMask.push_back("Player");
	m_Shoot->m_TargetTag = "Enemy";
	m_Shoot->m_Ammo = 9999;

	m_Controller = m_Owner->m_ComponentManager.AddComponent<Controller>();
	m_Controller->SetPawn(this);

	m_MuzzleFlash.m_GameObject = m_Owner->GetChilds()[0];
	m_MuzzleFlash.m_RelatedPosition = m_MuzzleFlash.m_GameObject->m_Transform.GetPosition() - m_Owner->m_Transform.GetPosition();
	m_MuzzleFlash.m_Renderer2D = m_MuzzleFlash.m_GameObject->m_ComponentManager.GetComponent<Renderer2D>();

	m_Skills = m_Owner->m_ComponentManager.AddComponent<Skills>();
	m_Skills->m_Player = this;
}

void Player::OnUpdate()
{
	if (m_Health->IsDead())
	{
		m_A2d->SetAutoUV(true);
		return;
	}

	Movement();

	Environment::GetInstance().GetMainCamera()->m_Transform.Translate(GetOwner()->m_Transform.GetPosition());

	ICollider2D* collider = m_Bc2d->IntersectTrigger();
	if (collider)
	{
		if (collider->m_Tag == "Ammo")
		{
			SoundManager::GetInstance().Play(m_PickUpSound);
			m_Shoot->m_Ammo += 10;
			m_Owner->GetScene()->DeleteGameObject(collider->GetOwner());
		}
	}
}

void Player::OnClose()
{

}

IComponent* Player::Create(GameObject* owner)
{
	return new Player();
}

Pengine::IComponent* Player::New(Pengine::GameObject* owner)
{
	return Create(owner);
}
