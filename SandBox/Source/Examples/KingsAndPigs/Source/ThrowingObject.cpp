#include "ThrowingObject.h"

#include "EventSystem/EventSystem.h"
#include "Events/OnMainThreadCallback.h"
#include "Core/GameObject.h"
#include "Core/Timer.h"
#include "Components/BoxCollider2D.h"

using namespace Pengine;

IComponent* ThrowingObject::New(GameObject* owner)
{
	return Create(owner);
}

void ThrowingObject::Copy(const IComponent& component)
{
	COPY_PROPERTIES(ThrowingObject, component)

	ThrowingObject& throwingObject = *(ThrowingObject*)&component;
	m_Type = component.GetType();
}

ThrowingObject::~ThrowingObject()
{
	EventSystem::GetInstance().UnregisterAll(this);
}

IComponent* ThrowingObject::Create(GameObject* owner)
{
	return new ThrowingObject();
}

void ThrowingObject::OnUpdate()
{
	if (!m_BlewUp && m_Rb2d)
	{
		if (m_Rb2d->IsCollided())
		{
			Blow();
			GetOwner()->m_ComponentManager.RemoveComponent<Renderer2D>();
		}
	}
}

void ThrowingObject::OnThrow()
{
	if (!GetOwner()) return;

	m_A2d = GetOwner()->m_ComponentManager.AddComponent<Animator2D>();
	m_Ic2d = GetOwner()->m_ComponentManager.GetComponentSubClass<ICollider2D>();
	m_Rb2d = GetOwner()->m_ComponentManager.AddComponent<Rigidbody2D>();

	if (!m_BlowOnCollide)
	{
		Timer::SetCallback([this]
			{
				if (IsValid(this))
				{
					Blow();
				}
			}
			, m_TimeToBlow);
		}
	else
	{
		Timer::SetCallback([this]
			{
				EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
			}
		, 0.35f);
	}
}

void ThrowingObject::Blow()
{
	if (!m_Ic2d) return;

	m_BlewUp = true;
	GetOwner()->m_ComponentManager.RemoveComponent<Rigidbody2D>();
	m_Ic2d->SetTrigger(true);
	m_A2d->ResetTime();
	m_A2d->SetCurrentAnimation(m_A2d->GetAnimation("Blow") ?
		m_A2d->GetAnimation("Blow") : m_A2d->GetAnimation("Death"));
	((BoxCollider2D*)m_Ic2d)->SetSize(m_DamageArea);

	m_ExplosionSound = SoundManager::GetInstance().Load("Source/Examples/KingsAndPigs/Sounds/Explosion.ogg");
	SoundManager::GetInstance().Play(m_ExplosionSound);

	auto callback = [this]
	{
		if (m_CheckID == 0)
		{
			m_Ic2d->SetTrigger(false);
		}
	};
	EventSystem::GetInstance().SendCallbackOnFrame(callback, 2);

	m_A2d->AddEndCallback("DeleteLater", [this]
		{
			GetOwner()->DeleteLater();

			return true;
		}
	);

	GetOwner()->ForChilds([this](GameObject& child)
		{
			child.SetEnabled(true);
			child.m_ComponentManager.AddComponent<BoxCollider2D>()->SetTag("Wall");
			Rigidbody2D* rb2d = child.m_ComponentManager.AddComponent<Rigidbody2D>();
			rb2d->SetStatic(false);

			glm::vec3 direction = glm::normalize(child.m_Transform.GetPosition() - GetOwner()->m_Transform.GetPosition());
			Timer::SetCallback([this, direction, rb2d]
				{
					rb2d->ApplyLinearImpulseToCenter(glm::vec2(direction) * m_BlowImpulse, true);
				}
			, 0.1f);
		}
	);

	std::vector<GameObject*> childs = GetOwner()->GetChilds();
	for (size_t i = 0; i < childs.size(); i++)
	{
		GetOwner()->RemoveChild(childs[i]);
	}

	for (size_t i = 0; i < childs.size(); i++)
	{
		childs[i]->DeleteLater(2.0f);
	}
}
