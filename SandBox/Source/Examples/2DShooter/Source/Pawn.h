#pragma once

#include "Core/Core.h"
#include "EventSystem/Listener.h"
#include "Core/GameObject.h"
#include "Core/Component.h"
#include "Components/Animator2D.h"
#include "Components/BoxCollider2D.h"
#include "Components/Renderer2D.h"
#include "Components/Rigidbody2D.h"

class Pawn : public Pengine::IListener, public Pengine::IComponent
{
	RTTR_ENABLE(IComponent)

public:

	class Health* m_Health = nullptr;
	class Shoot* m_Shoot = nullptr;
	class Controller* m_Controller = nullptr;
	class Pengine::Animator2D* m_A2d = nullptr;
	class Pengine::Renderer2D* m_R2d = nullptr;
	class Pengine::BoxCollider2D* m_Bc2d = nullptr;
	class Pengine::Rigidbody2D* m_Rb2d = nullptr;

	public: PROPERTY(Pawn, float, m_Money, 100.0f)
	public: PROPERTY(Pawn, float, m_Price, 200.0f)
public:

	struct MuzzleFlash
	{
		class Pengine::GameObject* m_GameObject = nullptr;
		class Pengine::Renderer2D* m_Renderer2D = nullptr;
		glm::vec3 m_RelatedPosition;
	} m_MuzzleFlash;

	static class Pengine::IComponent* Create(class Pengine::GameObject* owner)
	{
		return new Pawn();
	}

	virtual class Pengine::IComponent* New(class Pengine::GameObject* owner) override
	{
		return Create(owner);
	}

	virtual void Copy(const Pengine::IComponent& component) override
	{
		Pawn& pawn = *(Pawn*)&component;
		m_Type = component.GetType();
	}

	Pawn() = default;
	virtual ~Pawn() = default;
};
REGISTER_CLASS(Pawn)