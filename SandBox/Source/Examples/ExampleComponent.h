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

	virtual void Move(IComponent&& component) override;

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

	ExampleComponent(const ExampleComponent& a2d);

	ExampleComponent(ExampleComponent&& a2d) noexcept;

	ExampleComponent& operator=(const ExampleComponent& a2d);

	ExampleComponent& operator=(ExampleComponent&& a2d) noexcept;
};