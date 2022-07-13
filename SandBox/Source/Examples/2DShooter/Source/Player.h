#pragma once

#include "Core/Core.h"
#include "Core/GameObject.h"
#include "Components/Animator2D.h"
#include "Components/BoxCollider2D.h"
#include "Components/Renderer2D.h"
#include "Components/Rigidbody2D.h"
#include "Audio/Sound.h"

#include "Pawn.h"
#include "Skills.h"

class Player : public Pawn
{
	RTTR_ENABLE(Pawn)
private:
	
	Pengine::Sound* m_PickUpSound = nullptr;

	Skills* m_Skills = nullptr;
	
	void Movement();
public:

	Player();
	~Player();

	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnClose();

	static Pengine::IComponent* Create(Pengine::GameObject* owner);

	virtual Pengine::IComponent* New(Pengine::GameObject* owner) override;
};
REGISTER_CLASS(Player)