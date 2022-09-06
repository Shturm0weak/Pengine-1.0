#include "Pig.h"

#include "Core/GameObject.h"
#include "Core/ComponentManager.h"
#include "EventSystem/EventSystem.h"
#include "Core/Input.h"
#include "Core/Raycast2D.h"
#include "Core/Visualizer.h"
#include "Core/Scene.h"

using namespace Pengine;

void Pig::LookForEnemy()
{
	if (m_State != STATE::CHASE && IsHit())
	{
		m_State = STATE::CHASE;
		m_CurrentTargetPosition = GetLastHitInstigator();
		
		return;
	}

	glm::vec3 start = GetOwner()->m_Transform.GetPosition() + glm::vec3(0.0f, GetBc2d()->GetSize().y * 0.5f, 0.0f);
	glm::vec3 end = m_King->m_Transform.GetPosition();
	
	if (glm::distance(end, start) > m_VisibilityRange) return;

	Raycast2D::Hit2D hit;
	Raycast2D::Raycast(GetOwner()->GetScene(), start, end, hit, m_IgnoreAttackColliderTags);

	if (hit.GetCollider() && hit.GetCollider()->m_Tag == m_EnemyColliderTag)
	{
		//Visualizer::DrawLine(start, glm::vec3(hit.GetPosition(), 0.0f), { 0.0f, 0.0f, 1.0f, 1.0f });
		m_CurrentTargetPosition = hit.GetCollider()->GetOwner();
		float distance = hit.GetCollider()->GetPosition().x - GetBc2d()->GetPosition().x;

		if (m_State != STATE::ATTACK || abs(distance) > m_AttackRange)
		{
			m_State = STATE::CHASE;
		}
		else
		{
			//m_State = STATE::PATROL;
		}
	}
	else
	{
		//Visualizer::DrawLine(start, end, { 1.0f, 0.0f, 0.0f, 1.0f });
		m_State = STATE::PATROL;
	}
}

void Pig::Patrol()
{
	if (!m_StartPosition || !m_EndPosition) return;

	m_WaitTimer -= Time::GetDeltaTime();

	if (m_State == STATE::PATROL && m_CurrentTargetPosition)
	{
		if (m_CurrentTargetPosition != m_StartPosition && m_CurrentTargetPosition != m_EndPosition)
		{
			m_CurrentTargetPosition = m_StartPosition;
		}

		float distance = m_CurrentTargetPosition->m_Transform.GetPosition().x - m_Owner->m_Transform.GetPosition().x;
		int direction = glm::sign(distance);
		if (direction == 1)
		{
			MoveRight();
		}
		else if (direction == -1)
		{
			MoveLeft();
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
	}
}

void Pig::Chase()
{
	if (m_State != STATE::CHASE) return;

	BoxCollider2D* bc2d = nullptr;
	if (m_CurrentTargetPosition)
	{
		bc2d = m_CurrentTargetPosition->m_ComponentManager.GetComponent<BoxCollider2D>();
	}

	if (!bc2d) return;

	float distance = glm::distance(bc2d->GetPosition(), GetBc2d()->GetPosition());
	int direction = glm::sign(bc2d->GetPosition().x - GetBc2d()->GetPosition().x);
	if (direction == 1)
	{
		MoveRight();
	}
	else if (direction == -1)
	{
		MoveLeft();
	}

	if (abs(distance) <= m_AttackRange)
	{
		m_State = STATE::ATTACK;
	}
}

void Pig::Attack()
{
	if (m_State == STATE::ATTACK)
	{
		Controller::Attack();
	}
}

IComponent* Pig::New(GameObject* owner)
{
	return Create(owner);
}

void Pig::Copy(const IComponent& component)
{
	Controller::Copy(component);

	COPY_PROPERTIES(Pig, component)

	Pig& pig = *(Pig*)&component;
	m_Type = component.GetType();
}

IComponent* Pig::Create(GameObject* owner)
{
	return new Pig();
}

void Pig::OnStart()
{
	m_Direction = -1;

	if (m_InitializeDirection == 1)
	{
		MoveRight();
	}
	else if (m_InitializeDirection == -1)
	{
		MoveLeft();
	}

	m_StartPosition = GetOwner()->GetChildByName("StartPosition");
	m_EndPosition = GetOwner()->GetChildByName("EndPosition");

	m_King = GetOwner()->GetScene()->FindGameObject("King");
}

void Pig::Movement()
{
	if (!IsDead())
	{
		LookForEnemy();
		Chase();
		Patrol();
		Attack();

		if ((!m_CurrentTargetPosition || !m_CurrentTargetPosition->m_IsInitialized)
			|| (m_State == STATE::ATTACK && !IsAbleToAttack()))
		{
			Idle();
		}

		/*if (Input::Mouse::IsMousePressed(Keycode::MOUSE_BUTTON_1))
		{
			MeleeAttack();
		}
		else if (Input::KeyBoard::IsKeyPressed(Keycode::SPACE))
		{
			Jump();
		}
		else if (Input::KeyBoard::IsKeyDown(Keycode::KEY_D))
		{
			MoveRight();
		}
		else if (Input::KeyBoard::IsKeyDown(Keycode::KEY_A))
		{
			MoveLeft();
		}
		else
		{
		
		}*/
	}
}