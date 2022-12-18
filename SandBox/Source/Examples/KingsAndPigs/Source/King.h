#pragma once

#include "Core/Core.h"
#include "Core/Component.h"
#include "EventSystem/Listener.h"
#include "Components/Renderer2D.h"
#include "Components/Animator2D.h"
#include "Components/BoxCollider2D.h"
#include "Components/Rigidbody2D.h"

#include "Controller.h"

using namespace Pengine;

class King : public Controller
{
	REGISTER_PARENT_CLASS(Controller)

private:

	void ThrowingObjectCheck();

	void LeaveLevel();

	void EnterLevel();

	void PickUpHearts();

	Door* m_Door = nullptr;

	Sound* m_PickUpHeartSound = nullptr;

	bool m_IsLeavingLevel = false;
	bool m_IsStartedLeavingLevel = false;
	bool m_IsEnteringLevel = false;
protected:

	virtual void UpdateAnimation() override;

	virtual void Movement();
public:

	virtual IComponent* New(GameObject* owner) override;

	virtual void Copy(const IComponent& component) override;

	King();

	static IComponent* Create(GameObject* owner);

	virtual void OnStart() override;

	virtual void OnUpdate() override;
};
REGISTER_COMPONENT(King)