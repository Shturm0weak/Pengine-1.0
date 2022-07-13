#pragma once

#include "Core/Core.h"
#include "Core/GameObject.h"
#include "Components/Animator2D.h"
#include "Components/BoxCollider2D.h"
#include "Components/Renderer2D.h"
#include "Components/Rigidbody2D.h"

#include "Pawn.h"

class Enemy : public Pawn
{
	RTTR_ENABLE(Pawn)

private:

	class EnemyAI* m_EnemyAI = nullptr;
public:

	Enemy();
	~Enemy();

	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnClose();

	static Pengine::IComponent* Create(class Pengine::GameObject* owner);

	virtual Pengine::IComponent* New(class Pengine::GameObject* owner) override;
};
REGISTER_CLASS(Enemy)