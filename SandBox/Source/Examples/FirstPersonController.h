#pragma once

#include "Core/Core.h"
#include "Core/Component.h"
#include "Core/ReflectionSystem.h"
#include "EventSystem/Listener.h"
#include "Components/Rigidbody3D.h"

using namespace Pengine;

class FirstPersonController : public IComponent, public IListener
{
	REGISTER_PARENT_CLASS(IComponent)
public: PROPERTY(float, m_MaxSpeed, 1.0f)
public: PROPERTY(float, m_Accelaration, 1.0f)
public: PROPERTY(float, m_CurrentSpeed, 0.0f)
private:

	Rigidbody3D* m_Rb3d = nullptr;

	void TranslateMovement();

	void PhysicsMovement();
public:

	FirstPersonController() = default;
	~FirstPersonController();

	static class IComponent* Create(class Pengine::GameObject* owner);

	virtual class IComponent* New(class GameObject* owner) override;

	virtual void Copy(const IComponent& component) override;

	virtual void OnRegisterClient() override;

	virtual void OnUpdate() override;

	virtual void OnStart() override;
};
REGISTER_COMPONENT(FirstPersonController)