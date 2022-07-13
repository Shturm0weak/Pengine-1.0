#include "Controller.h"

#include "Core/GameObject.h"
#include "EventSystem/EventSystem.h"
#include "Core/Raycast2D.h"
#include "Core/Scene.h"
#include "Core/Visualizer.h"
#include "Components/BoxCollider2D.h"

#include "Player.h"
#include "Shoot.h"

using namespace Pengine;

IComponent* Controller::New(GameObject* owner)
{
	return Create(owner);
}

void Controller::Copy(const IComponent& component)
{
	Controller& controller = *(Controller*)&component;
	m_Type = component.GetType();
}

void Controller::Update()
{
	m_IsLanded = IsLandedCalculate();

	OnDirectionChanged();
}

IComponent* Controller::Create(GameObject* owner)
{
	return new Controller();
}

bool Controller::IsLandedCalculate()
{
	std::vector<std::string> ignoreMask = { "Player", "Enemy" };
	glm::vec3 position = m_Pawn->GetOwner()->m_Transform.GetPosition();
	Raycast2D::Hit2D hits[3];
	Raycast2D::Raycast(m_Owner->GetScene(), position - m_Pawn->m_Bc2d->GetSize().x, position - m_Pawn->m_Bc2d->GetSize().x + glm::vec3(-0.1f, -1000.0f, 0.0f), hits[0], ignoreMask);
	Raycast2D::Raycast(m_Owner->GetScene(), position, position + glm::vec3(0.0f, -1000.0f, 0.0f), hits[1], ignoreMask);
	Raycast2D::Raycast(m_Owner->GetScene(), position + m_Pawn->m_Bc2d->GetSize().x, position + m_Pawn->m_Bc2d->GetSize().x + glm::vec3(0.1f, -1000.0f, 0.0f), hits[2], ignoreMask);

	for (auto hit : hits)
	{
		if (hit.GetCollider())
		{
			float distance = glm::abs(hit.GetPosition().y - position.y);
			if (hit.GetCollider()->m_Tag == "Ground" && distance < m_Pawn->m_Bc2d->GetSize().y * 1.1f)
			{
				//Visualizer::DrawLine(position, { hit.m_Position, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });

				if (!m_IsLanded)
				{
					m_Pawn->m_A2d->Reset();
				}

				return true;
			}
		}
	}

	if (m_IsLanded)
	{
		m_Pawn->m_A2d->Reset();
	}

	return false;
}

void Controller::OnDirectionChanged()
{
	if (m_Direction != m_PreviousDirection)
	{
		m_Pawn->m_A2d->Reset();
		if (m_Direction == 1)
		{
			m_Pawn->m_MuzzleFlash.m_GameObject->m_Transform.Translate(m_Owner->m_Transform.GetPosition()
				+ glm::vec3(m_Pawn->m_MuzzleFlash.m_RelatedPosition.x, m_Pawn->m_MuzzleFlash.m_RelatedPosition.y, 0.0f));
			m_Pawn->m_MuzzleFlash.m_Renderer2D->OriginalUV();
		}
		else
		{
			m_Pawn->m_MuzzleFlash.m_GameObject->m_Transform.Translate(m_Owner->m_Transform.GetPosition()
				- glm::vec3(m_Pawn->m_MuzzleFlash.m_RelatedPosition.x, -m_Pawn->m_MuzzleFlash.m_RelatedPosition.y, 0.0f));
			m_Pawn->m_MuzzleFlash.m_Renderer2D->ReversedUV();
		}
		m_PreviousDirection = m_Direction;
	}
}

void Controller::MoveRight()
{
	m_Owner->m_Transform.Translate(m_Owner->m_Transform.GetPosition()
		+ glm::vec3(m_Speed * Time::GetDeltaTime(), 0.0f, 0.0f));
	m_Direction = 1;
	if (m_State == 0)
	{
		m_Pawn->m_A2d->Reset();
	}
	m_State = 1;
	m_Pawn->m_R2d->SetUV(m_Pawn->m_A2d->GetOriginalUV());
	m_Pawn->m_A2d->SetCurrentAnimation(m_IsLanded ? m_Pawn->m_A2d->m_Animations[1] : m_Pawn->m_A2d->m_Animations[3]);
}

void Controller::MoveLeft()
{
	m_Owner->m_Transform.Translate(m_Owner->m_Transform.GetPosition()
		- glm::vec3(m_Speed * Time::GetDeltaTime(), 0.0f, 0.0f));
	m_Direction = -1;
	if (m_State == 0)
	{
		m_Pawn->m_A2d->Reset();
	}
	m_State = 1;
	m_Pawn->m_R2d->SetUV(m_Pawn->m_A2d->GetReversedUV());
	m_Pawn->m_A2d->SetCurrentAnimation(m_IsLanded ? m_Pawn->m_A2d->m_Animations[1] : m_Pawn->m_A2d->m_Animations[3]);
}

void Controller::Idle()
{
	if (m_State != 0)
	{
		m_Pawn->m_A2d->Reset();
	}
	m_State = 0;
	m_Direction == 1 ? m_Pawn->m_R2d->SetUV(m_Pawn->m_A2d->GetOriginalUV()) : m_Pawn->m_R2d->SetUV(m_Pawn->m_A2d->GetReversedUV());
	m_Pawn->m_A2d->SetCurrentAnimation(m_IsLanded ? m_Pawn->m_A2d->m_Animations[0] : m_Pawn->m_A2d->m_Animations[3]);
}

void Controller::Jump()
{
	if (m_IsLanded)
	{
		m_IsLanded = false;
		if (m_State != 2)
		{
			m_Pawn->m_A2d->Reset();
		}
		m_State = 2;
		m_Pawn->m_Rb2d->GetBody()->ApplyLinearImpulseToCenter({ 0.0f, m_Jump }, true);
	}
}
