#include "King.h"

#include "Core/GameObject.h"
#include "Core/ComponentManager.h"
#include "EventSystem/EventSystem.h"
#include "Events/OnMainThreadCallback.h"
#include "Core/Input.h"
#include "Core/Scene.h"
#include "Core/Serializer.h"

#include "ThrowingObject.h"

using namespace Pengine;

IComponent* King::New(GameObject* owner)
{
	return Create(owner);
}

void King::Copy(const IComponent& component)
{
	Controller::Copy(component);

	COPY_PROPERTIES(King, component)

	King& king = *(King*)&component;
	m_Type = component.GetType();
}

King::King()
{
	std::string directory = "Source/Examples/KingsAndPigs/Sounds/";

	m_PickUpHeartSound = SoundManager::GetInstance().Load(directory + "PickUpHeart.wav");
}

IComponent* King::Create(GameObject* owner)
{
	return new King();
}

void King::ThrowingObjectCheck()
{
	if (ICollider2D* collider = GetBc2d()->IntersectTrigger())
	{
		if (collider->m_Tag == "ThrowingObject")
		{
			ThrowingObject* throwingObject = collider->GetOwner()->m_ComponentManager.GetComponent<ThrowingObject>();
			Hit(throwingObject->m_Damage);
		}
	}
}

void King::EnterLevel()
{
	m_IsEnteringLevel = true;

	GetA2d()->Reset();
	GetA2d()->m_EndCallbacks.push_back([this]()
		{
			m_IsEnteringLevel = false;

			return true;
		}
	);
}

void King::PickUpHearts()
{
	if (ICollider2D* collider = GetBc2d()->IntersectTrigger())
	{
		if (collider->m_Tag == "Heart" && GetHealth()->GetCurrentHealth() < GetHealth()->GetMaxHealth())
		{
			GetHealth()->SetCurrentHealth(GetHealth()->GetCurrentHealth() + 1);
			collider->GetOwner()->DeleteLater();

			SoundManager::GetInstance().Play(m_PickUpHeartSound);
		}
	}
}

void King::LeaveLevel()
{
	if (GetBc2d())
	{
		ICollider2D* collider = GetBc2d()->IntersectTrigger();
		if (collider && collider->m_Tag == "Door")
		{
			m_IsLeavingLevel = true;

			m_Door->Open();

			if (!m_IsStartedLeavingLevel)
			{
				GetOwner()->m_Transform.Translate(glm::vec3(collider->GetOwner()->m_Transform.GetPosition().x,
					GetOwner()->m_Transform.GetPosition().y, 0.0f));

				m_Door->GetA2d()->Reset();

				GetA2d()->Reset();
				GetA2d()->m_EndCallbacks.push_back([this]
					{
						bool isLeavingLevel = m_IsLeavingLevel;
						auto callback = [this, isLeavingLevel]
						{
							if (!isLeavingLevel) return;

							if (GetOwner() && GetOwner()->GetScene())
							{
								std::string directory = "Source/Examples/KingsAndPigs/Levels/" +
									m_Door->m_LevelName + ".yaml";
								GetOwner()->GetScene()->Clear();
								Serializer::DeserializeScene(directory)->OnRegisterClients();
								EventSystem::GetInstance().SendEvent(new IEvent(EventType::ONSTART));
							}
						};
						EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));

						return true;
					}
				);
			}
		}
		else
		{
			Idle();
		}
	}
}

void King::UpdateAnimation()
{
	std::vector<float> uv;
	if (m_Direction == 1)
	{
		uv = m_UVReversedByDefault ? GetA2d()->GetReversedUV() : GetA2d()->GetOriginalUV();
	}
	else if (m_Direction == -1)
	{
		uv = m_UVReversedByDefault ? GetA2d()->GetOriginalUV() : GetA2d()->GetReversedUV();
	}

	Animation2DManager::Animation2D* animation;

	if (m_IsEnteringLevel)
	{
		animation = GetA2d()->GetAnimation("DoorOut");
	}
	else if (IsDead())
	{
		animation = GetA2d()->GetAnimation("Death");
	}
	else if (m_IsLeavingLevel)
	{
		animation = GetA2d()->GetAnimation("DoorIn");
	}
	else if (IsHit())
	{
		animation = GetA2d()->GetAnimation("Hit");
	}
	else if (IsAttacking())
	{
		animation = GetA2d()->GetAnimation("Attack");
	}
	else if (IsLanded())
	{
		if (IsIdle())
		{
			animation = GetA2d()->GetAnimation("Idle");
		}
		else
		{
			animation = GetA2d()->GetAnimation("Run");
		}
	}
	else
	{
		if (IsFalling())
		{
			animation = GetA2d()->GetAnimation("Fall");
		}
		else
		{
			animation = GetA2d()->GetAnimation("Jump");
		}
	}

	GetR2d()->SetUV(uv);
	GetA2d()->SetCurrentAnimation(animation);
}

void King::Movement()
{
	if (!IsDead() && !m_IsEnteringLevel)
	{
		if (Input::Mouse::IsMousePressed(Keycode::MOUSE_BUTTON_1))
		{
			Attack();
		}
		else if (Input::KeyBoard::IsKeyPressed(Keycode::SPACE))
		{
			Jump();
		}

		if (Input::KeyBoard::IsKeyDown(Keycode::KEY_L))
		{
			LeaveLevel();
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
			Idle();
		}
	}
}

void King::OnUpdate()
{
	m_IsStartedLeavingLevel = m_IsLeavingLevel;
	m_IsLeavingLevel = false;

	ThrowingObjectCheck();
	PickUpHearts();
	GetHealth()->DrawHearts(IsHit());

	Controller::OnUpdate();
}

void King::OnStart()
{
	if (GameObject* door = GetOwner()->GetScene()->FindGameObjectByName("Door"))
	{
		m_Door = door->m_ComponentManager.GetComponent<Door>();
	}

	if (GameObject* entranceDoor = GetOwner()->GetScene()->FindGameObjectByName("EntranceDoor"))
	{
		GetOwner()->m_Transform.Translate(entranceDoor->m_Transform.GetPosition());

		if (Animator2D* a2d = entranceDoor->m_ComponentManager.GetComponent<Animator2D>())
		{
			auto callback = [a2d]
			{
				a2d->Reset();
				a2d->SetCurrentAnimation(a2d->GetAnimation("Close"));

				a2d->m_EndCallbacks.push_back([a2d]()
					{
						a2d->SetCurrentAnimation(a2d->GetAnimation("Idle"));

						return true;
					}
				);
			};
			EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
		}
	}

	EnterLevel();
}