#pragma once

#include "Core/Core.h"
#include "Core/Component.h"

using namespace Pengine;

class ExampleComponent : public IComponent
{
public:

	virtual IComponent* New(GameObject* owner) override
	{
		return Create(owner);
	}

	virtual void Copy(const IComponent& component) override
	{
		ExampleComponent& exampleComponent = *(ExampleComponent*)&component;
		m_Type = component.GetType();
	}

	ExampleComponent() = default;
	~ExampleComponent() = default;

	static IComponent* Create(GameObject* owner)
	{
		ExampleComponent* exampleComponent = new ExampleComponent();
		return exampleComponent;
	}
};