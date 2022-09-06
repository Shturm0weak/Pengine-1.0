#pragma once

#include "Core/Core.h"
#include "Core/Component.h"
#include "EventSystem/Listener.h"

#include "Controller.h"

using namespace Pengine;

class Box : public Controller
{
	RTTR_ENABLE(Controller)

private:

	  GameObject* m_HeartPrefab = nullptr;

	  int m_DropedHearts = 0;
	  int m_MaxDropedHearts = 1;
public:

	virtual IComponent* New(GameObject* owner) override;

	virtual void Copy(const IComponent& component) override;

	static IComponent* Create(GameObject* owner);

	virtual void OnUpdate() override;

	virtual void OnStart() override;
};
REGISTER_CLASS(Box)