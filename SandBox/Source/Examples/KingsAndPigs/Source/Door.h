#pragma once

#include "Core/Core.h"
#include "Core/Component.h"
#include "EventSystem/Listener.h"
#include "Components/Animator2D.h"

using namespace Pengine;

class Door : public IComponent, public IListener
{
	REGISTER_PARENT_CLASS(IComponent)

public: PROPERTY(std::string, m_LevelName, "")
private:

	Animator2D* m_A2d = nullptr;

	bool m_IsOpenning = false;
public:

	virtual IComponent* New(GameObject* owner) override;

	virtual void Copy(const IComponent& component) override;

	~Door();

	static IComponent* Create(GameObject* owner);

	virtual void OnRegisterClient() override;

	virtual void OnUpdate() override;

	virtual void OnStart() override;

	Animator2D* GetA2d() const { return m_A2d; }

	void Open();
};
REGISTER_COMPONENT(Door)