#include "PigThrowing.h"

#include "Core/Scene.h"
#include "Core/Raycast2D.h"
#include "Core/Timer.h"

#include "ThrowingObject.h"

void PigThrowing::Attack()
{
	if (m_State == STATE::ATTACK)
	{
		if (!IsAttacking() && !IsHit() && IsAbleToAttack())
		{
			AttackStart();

			auto throwObject = [this]
			{
				GameObject* object = GetOwner()->GetScene()->CreateGameObject();
				object->Copy(*m_ThrowingObjectPrefab);
				object->m_Transform.Translate(glm::vec3(0.0f, 0.35f, 0.0f)
					+ GetOwner()->m_Transform.GetPosition());
				object->SetName("ThrowingObject");
				object->SetEnabled(true);

				ThrowingObject* throwingObject = object->m_ComponentManager.GetComponent<ThrowingObject>();
				throwingObject->OnThrow();

				Timer::SetCallback([this, object] {
					if (Rigidbody2D* rb2d = object->m_ComponentManager.GetComponent<Rigidbody2D>())
					{
						rb2d->ApplyLinearImpulseToCenter(glm::vec2(GetDirection() *
							m_AttackImpulse.x, m_AttackImpulse.y), true);
					}
				}, 0.1f);
			};

			AttackEnd(throwObject);
		}
	}
}

void PigThrowing::UpdateAnimation()
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

	if (IsDead())
	{
		animation = GetA2d()->GetAnimation("Death");
		GetOwner()->m_Transform.Scale(m_PigScale);
	}
	else if (IsHit())
	{
		animation = GetA2d()->GetAnimation("Hit");
		GetOwner()->m_Transform.Scale(m_PigScale);
	}
	else if (IsAttacking())
	{
		animation = GetA2d()->GetAnimation("Throw");
		GetOwner()->m_Transform.Scale(m_ThrowingPigScale);
	}
	else if (m_IsPicking)
	{
		animation = GetA2d()->GetAnimation("Pick");
		GetOwner()->m_Transform.Scale(m_ThrowingPigScale);
	}
	else if (IsLanded())
	{
		if (IsIdle())
		{
			animation = GetA2d()->GetAnimation("Idle");
			GetOwner()->m_Transform.Scale(m_ThrowingPigScale);
		}
		else
		{
			animation = GetA2d()->GetAnimation("Run");
			GetOwner()->m_Transform.Scale(m_ThrowingPigScale);
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

IComponent* PigThrowing::New(GameObject* owner)
{
	return Create(owner);
}

void PigThrowing::Copy(const IComponent& component)
{
	Pig::Copy(component);

	COPY_PROPERTIES(PigThrowing, component)

	PigThrowing& pigThrowing = *(PigThrowing*)&component;
	m_Type = component.GetType();
}

IComponent* PigThrowing::Create(GameObject* owner)
{
	return new PigThrowing();
}

void PigThrowing::OnStart()
{
	Pig::OnStart();

	m_ThrowingObjectPrefab = GetOwner()->GetScene()->FindGameObject(m_ObjectName);
}
