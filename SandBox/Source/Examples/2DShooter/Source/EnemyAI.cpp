#include "EnemyAI.h"

#include "EventSystem/EventSystem.h"
#include "Core/GameObject.h"
#include "Core/Raycast2D.h"
#include "Core/Visualizer.h"

#include "Controller.h"
#include "Shoot.h"

using namespace Pengine;

void EnemyAI::Patrol()
{
	m_WaitTimer -= Time::GetDeltaTime();

	if (m_State == STATE::PATROL && m_CurrentTargetPosition)
	{
		float distance = m_CurrentTargetPosition->m_Transform.GetPosition().x - m_Owner->m_Transform.GetPosition().x;
		int direction = glm::sign(distance);
		if (direction == 1)
		{
			m_Controller->MoveRight();
		}
		else if (direction == -1)
		{
			m_Controller->MoveLeft();
		}

		if (abs(distance) < 0.2f)
		{
			m_PreviousTargetPosition = m_CurrentTargetPosition;
			m_CurrentTargetPosition = nullptr;
			m_WaitTimer = m_TimeToWait;
		}
	}
	else
	{
		if (m_WaitTimer < 0.0f)
		{
			m_CurrentTargetPosition = m_PreviousTargetPosition == m_StartPosition ? m_EndPosition : m_StartPosition;
		}

		m_Controller->Idle();
	}
}

void EnemyAI::Vision()
{
	std::vector<std::string> ignoreMaskByTag;
	std::vector<ICollider2D*> ignoreMaskByCollider = { m_Owner->m_ComponentManager.GetComponent<ICollider2D>() };
	glm::vec3 position = m_Owner->m_Transform.GetPosition() + glm::vec3(0.0f, 0.2f, 0.0f);
	Raycast2D::Hit2D hit;
	Raycast2D::Raycast(m_Owner->GetScene(), position, position + (float)m_Controller->GetDirection() * glm::vec3(m_VisionLength, 0.0f, 0.0f), hit, ignoreMaskByTag, ignoreMaskByCollider);

	if (hit.GetCollider())
	{
		Visualizer::DrawLine(position, { hit.GetPosition(), 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f });
		if (hit.GetCollider()->m_Tag == "Player")
		{
			m_State = STATE::FIGHT;
		}
		else
		{
			m_State = STATE::PATROL;
		}
	}
	else
	{
		m_State = STATE::PATROL;
	}
}

void EnemyAI::Fight()
{
	if (m_State == STATE::FIGHT)
	{
		m_Shoot->Fire();
	}
}

IComponent* EnemyAI::New(GameObject* owner)
{
	return Create(owner);
}

void EnemyAI::Copy(const IComponent& component)
{
	EnemyAI& enemyAI = *(EnemyAI*)&component;
	m_Type = component.GetType();
}

void EnemyAI::OnStart()
{
	std::vector<GameObject*> childs = m_Owner->GetChilds();
	m_StartPosition = childs[1];
	m_EndPosition = childs[2];

	m_CurrentTargetPosition = m_StartPosition;
}

void EnemyAI::OnUpdate()
{
	Patrol();
	Vision();
	Fight();
}

void EnemyAI::OnClose()
{

}

EnemyAI::EnemyAI()
{
	EventSystem::GetInstance().RegisterClient(EventType::ONSTART, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONCLOSE, this);
}

EnemyAI::~EnemyAI()
{
	EventSystem::GetInstance().UnregisterAll(this);
}

IComponent* EnemyAI::Create(GameObject* owner)
{
	return new EnemyAI();
}
