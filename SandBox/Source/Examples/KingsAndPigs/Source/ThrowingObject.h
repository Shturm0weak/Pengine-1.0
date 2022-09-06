#pragma once

#include "Core/Core.h"
#include "Core/Component.h"
#include "EventSystem/Listener.h"
#include "Components/Animator2D.h"
#include "Components/CircleCollider2D.h"
#include "Components/Rigidbody2D.h"
#include "Components/Renderer2D.h"
#include "Audio/SoundManager.h"

using namespace Pengine;

class ThrowingObject : public IComponent, public IListener
{
	RTTR_ENABLE(IComponent)

public: PROPERTY(ThrowingObject, int, m_Damage, 1)
public: PROPERTY(ThrowingObject, float, m_TimeToBlow, 1.0f)
public: PROPERTY(ThrowingObject, bool, m_BlowOnCollide, false)
public: PROPERTY(ThrowingObject, glm::vec2, m_DamageArea, { 0.25f COM 0.25f })
public: PROPERTY(ThrowingObject, glm::vec2, m_BlowImpulse, { 0.25f COM 0.25f })
private:

	void Blow();

	Animator2D* m_A2d = nullptr;
	ICollider2D* m_Ic2d = nullptr;
	Rigidbody2D* m_Rb2d = nullptr;

	Sound* m_ExplosionSound = nullptr;

	bool m_BlewUp = false;
public:

	virtual IComponent* New(GameObject* owner) override;

	virtual void Copy(const IComponent& component) override;

	~ThrowingObject();

	static IComponent* Create(GameObject* owner);

	virtual void OnUpdate() override;

	void OnThrow();
};
REGISTER_CLASS(ThrowingObject)