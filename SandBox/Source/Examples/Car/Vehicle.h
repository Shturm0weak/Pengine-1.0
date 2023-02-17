#pragma once

#include "Core/Core.h"
#include "Core/Component.h"
#include "Core/ReflectionSystem.h"
#include "Core/ComponentManager.h"
#include "Core/GameObject.h"

using namespace Pengine;

class Vehicle : public IComponent
{
	REGISTER_PARENT_CLASS(IComponent)
public: PROPERTY(GameObject*, m_GameObject, false)
protected:

	virtual void Copy(const IComponent& component) override;

	virtual void Move(IComponent&& component) override;

	virtual IComponent* New(GameObject* owner) override;
public:

	static IComponent* Create(GameObject* owner);

	Vehicle() = default;

	~Vehicle() override;

	Vehicle(const Vehicle & a2d);

	Vehicle(Vehicle && a2d) noexcept;

	Vehicle& operator=(const Vehicle & a2d);

	Vehicle& operator=(Vehicle && a2d) noexcept;
};
REGISTER_COMPONENT(Vehicle)