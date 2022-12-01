#include "Box.h"

#include "Core/GameObject.h"
#include "Core/ComponentManager.h"
#include "EventSystem/EventSystem.h"
#include "Core/Input.h"
#include "Core/Raycast2D.h"
#include "Core/Visualizer.h"
#include "Core/Timer.h"
#include "Core/Scene.h"

using namespace Pengine;

IComponent* Box::New(GameObject* owner)
{
	return Create(owner);
}

void Box::Copy(const IComponent& component)
{
	Controller::Copy(component);

	COPY_PROPERTIES(Box, component)

	Box& box = *(Box*)&component;
	m_Type = component.GetType();
}

IComponent* Box::Create(GameObject* owner)
{
	return new Box();
}

void Box::OnUpdate()
{
	if (IsDead())
	{
		if (m_DropedHearts < m_MaxDropedHearts)
		{
			m_DropedHearts++;

			GameObject* heart = GetOwner()->GetScene()->CreateGameObject();
			heart->Copy(*m_HeartPrefab);
			heart->m_Transform.Translate(GetOwner()->m_Transform.GetPosition());
			heart->SetName("Heart");
			heart->SetEnabled(true);
		}

		GetOwner()->ForChilds([this](GameObject& child)
			{
				child.SetEnabled(true);
				child.m_ComponentManager.AddComponent<BoxCollider2D>()->m_Tag = "Wall";
				Rigidbody2D* rb2d = child.m_ComponentManager.AddComponent<Rigidbody2D>();
				rb2d->SetStatic(false);
				
				glm::vec3 direction = glm::normalize(child.m_Transform.GetPosition() - GetLastHitInstigator()->m_Transform.GetPosition());
				Timer::SetCallback([direction, rb2d]
					{
					rb2d->ApplyLinearImpulseToCenter(direction * 0.25f, true);
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
	else
	{
		UpdateParams();

		if (!IsHit())
		{
			Idle();
		}

		UpdateAnimation();
	}
}

void Box::OnStart()
{
	m_HeartPrefab = GetOwner()->GetScene()->FindGameObjectByName("HeartPrefab");
}