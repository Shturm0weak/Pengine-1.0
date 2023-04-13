#pragma once

#include "Core/Core.h"
#include "Core/Component.h"

using namespace Pengine;

class ExampleComponent : public IComponent
{
protected:

	virtual void Copy(const IComponent& component) override
	{
		ExampleComponent& exampleComponent = *(ExampleComponent*)&component;
		m_Type = component.GetType();
	}

	virtual IComponent* New(GameObject* owner) override
	{
		return Create(owner);
	}
public:

	static IComponent* Create(GameObject* owner)
	{
		ExampleComponent* exampleComponent = new ExampleComponent();
		return exampleComponent;
	}

	ExampleComponent() = default;

	~ExampleComponent() override;

	ExampleComponent(const ExampleComponent& exampleComponent);

	ExampleComponent& operator=(const ExampleComponent& exampleComponent);
};