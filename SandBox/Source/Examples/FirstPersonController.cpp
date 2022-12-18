#include "FirstPersonController.h"

#include "EventSystem/EventSystem.h"
#include "Core/Environment.h"
#include "Core/GameObject.h"
#include "Core/Input.h"

void FirstPersonController::TranslateMovement()
{
	glm::vec3 position = GetOwner()->m_Transform.GetPosition();
	std::shared_ptr<Camera> camera = Environment::GetInstance().GetMainCamera();
	glm::vec3 forward = camera->m_Transform.GetForward();
	glm::vec3 right = camera->m_Transform.GetRight();
	float speed = m_MaxSpeed;

	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_LEFT_SHIFT))
	{
		speed *= 2.0f;
	}

	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_W))
	{
		GetOwner()->m_Transform.Translate(forward * speed * Time::GetDeltaTime() + position);
	}
	else if (Input::KeyBoard::IsKeyDown(Keycode::KEY_S))
	{
		GetOwner()->m_Transform.Translate(forward * -speed * Time::GetDeltaTime() + position);
	}

	position = GetOwner()->m_Transform.GetPosition();

	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_D))
	{
		GetOwner()->m_Transform.Translate(right * speed * Time::GetDeltaTime() + position);
	}
	else if (Input::KeyBoard::IsKeyDown(Keycode::KEY_A))
	{
		GetOwner()->m_Transform.Translate(right * -speed * Time::GetDeltaTime() + position);
	}

	camera->m_Transform.Translate(GetOwner()->m_Transform.GetPosition());
}

void FirstPersonController::PhysicsMovement()
{
	float accelaration = m_Accelaration * Time::GetDeltaTime();
	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_LEFT_SHIFT))
	{
		accelaration *= 2.0f;
	}

	if (!m_Rb3d) return;

	glm::vec3 position = GetOwner()->m_Transform.GetPosition();
	std::shared_ptr<Camera> camera = Environment::GetInstance().GetMainCamera();
	glm::vec3 forward = camera->m_Transform.GetForward();
	glm::vec3 right = camera->m_Transform.GetRight();

	btVector3 linearVelocity = m_Rb3d->GetBody().getLinearVelocity();
	glm::vec3 velocity = { 0.0f, 0.0f, 0.0f };

	bool down = false;

	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_W))
	{
		down = true;
		velocity = forward * m_CurrentSpeed * Time::GetDeltaTime() + velocity;
	}
	else if (Input::KeyBoard::IsKeyDown(Keycode::KEY_S))
	{
		down = true;
		velocity = forward * -m_CurrentSpeed * Time::GetDeltaTime() + velocity;
	}

	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_D))
	{
		down = true;
		velocity = right * m_CurrentSpeed * Time::GetDeltaTime() + velocity;
	}
	else if (Input::KeyBoard::IsKeyDown(Keycode::KEY_A))
	{
		down = true;
		velocity = right * -m_CurrentSpeed * Time::GetDeltaTime() + velocity;
	}

	if (down)
	{
		m_CurrentSpeed = glm::clamp(m_CurrentSpeed + accelaration, 0.0f, m_MaxSpeed);
	}
	else
	{
		m_CurrentSpeed = glm::clamp(m_CurrentSpeed - accelaration, 0.0f, m_MaxSpeed);
	}

	m_Rb3d->GetBody().setLinearVelocity(btVector3(velocity.x, 0.0f, velocity.z) + linearVelocity);
	m_Rb3d->GetBody().setAngularFactor(0.0f);
	camera->m_Transform.Translate(GetOwner()->m_Transform.GetPosition());
}

FirstPersonController::~FirstPersonController()
{
	EventSystem::GetInstance().UnregisterAll(this);
}

IComponent* FirstPersonController::Create(GameObject* owner)
{
	return new FirstPersonController();
}

IComponent* FirstPersonController::New(GameObject* owner)
{
	return Create(owner);
}

void FirstPersonController::Copy(const IComponent& component)
{
	FirstPersonController& firstPersonController = *(FirstPersonController*)&component;
	COPY_PROPERTIES(firstPersonController)
	m_Type = component.GetType();
}

void FirstPersonController::OnRegisterClient()
{
	EventSystem::GetInstance().RegisterClient(EventType::ONSTART, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
}

void FirstPersonController::OnUpdate()
{
	PhysicsMovement();
}

void FirstPersonController::OnStart()
{
	m_Rb3d = GetOwner()->m_ComponentManager.AddComponent<Rigidbody3D>();
}
