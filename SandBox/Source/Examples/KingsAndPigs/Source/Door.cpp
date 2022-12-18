#include "Door.h"

#include "Core/GameObject.h"
#include "Core/ComponentManager.h"
#include "EventSystem/EventSystem.h"
#include "Core/Input.h"
#include "Core/Raycast2D.h"
#include "Core/Visualizer.h"
#include "Core/Timer.h"
#include "Core/Scene.h"

using namespace Pengine;

IComponent* Door::New(GameObject* owner)
{
	return Create(owner);
}

void Door::Copy(const IComponent& component)
{
	Door& door = *(Door*)&component;
	COPY_PROPERTIES(door)
	m_Type = component.GetType();
}

Door::~Door()
{
	EventSystem::GetInstance().UnregisterAll(this);
}

IComponent* Door::Create(GameObject* owner)
{
	return new Door();
}

void Door::OnRegisterClient()
{
	EventSystem::GetInstance().RegisterClient(EventType::ONSTART, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
}

void Door::OnUpdate()
{
	if (m_A2d && m_IsOpenning)
	{
		m_A2d->SetCurrentAnimation(m_A2d->GetAnimation("Open"));
	}
	else
	{
		m_A2d->SetCurrentAnimation(m_A2d->GetAnimation("Idle"));
	}

	m_IsOpenning = false;
}

void Door::OnStart()
{
	m_A2d = GetOwner()->m_ComponentManager.AddComponent<Animator2D>();
}

void Door::Open()
{
	m_IsOpenning = true;
}
