#pragma once

#include "Core/Core.h"
#include "Core/Component.h"
#include "Core/ReflectionSystem.h"
#include "Core/Input.h"
#include "Core/Time.h"
#include "Core/GameObject.h"
#include "Core/Scene.h"
#include "EventSystem/Listener.h"
#include "EventSystem/EventSystem.h"

using namespace Pengine;

class Rotator : public IComponent, public IListener
{
	REGISTER_PARENT_CLASS(IComponent)
private:
	
	bool m_Rotating = false;
protected:

	virtual void Copy(const IComponent& component) override
	{
		Rotator& rotator = *(Rotator*)&component;
		m_Type = component.GetType();
	}

	virtual IComponent* New(GameObject* owner) override
	{
		return Create(owner);
	}
public:

	static IComponent* Create(GameObject* owner)
	{
		Rotator* rotator = new Rotator();
		return rotator;
	}

	Rotator() = default;

	~Rotator() override
	{
		EventSystem::GetInstance().UnregisterAll(this);
	}

	Rotator(const Rotator& rotator);

	Rotator& operator=(const Rotator& rotator);

	virtual void OnRegisterClient() override
	{
		EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
	}

	virtual void OnUpdate() override
	{
		if (Input::KeyBoard::IsKeyPressed(Keycode::KEY_T))
		{
			m_Rotating = !m_Rotating;
		}

		if (m_Rotating)
		{
			GetOwner()->m_Transform.Rotate(GetOwner()->m_Transform.GetRotation() + glm::vec3(0.5f, 0.0f, 0.0f) * Time::GetDeltaTime());
		}
	}
};
REGISTER_COMPONENT(Rotator)