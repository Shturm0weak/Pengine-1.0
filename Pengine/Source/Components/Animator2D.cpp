#include "Animator2D.h"

#include "Renderer2D.h"
#include "../Core/GameObject.h"
#include "../EventSystem/EventSystem.h"

using namespace Pengine;

const char** Animator2D::GetAnimationsNames()
{
	if (m_AnimationsNames != nullptr)
	{
		delete[] m_AnimationsNames;
	}
	m_AnimationsNames = new const char*[m_Animations.size()];
	for(size_t i = 0; i < m_Animations.size(); i++)
	{
		m_AnimationsNames[i] = m_Animations[i]->m_Name.c_str();
	}

	return m_AnimationsNames;
}

IComponent* Animator2D::Create(GameObject* owner)
{
    return new Animator2D();
}

Animator2D::Animator2D()
{
	EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONSTART, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONCLOSE, this);
}

void Animator2D::OnUpdate()
{
	if (m_CurrentAnimation && m_Play)
	{
		PlayAnimation(m_CurrentAnimation);
	}
}

Animator2D::~Animator2D()
{
	EventSystem::GetInstance().UnregisterAll(this);
}

void Animator2D::OnStart()
{
	m_Play = true;
}

void Animator2D::OnClose()
{
	m_Play = false;
}

void Animator2D::Copy(const IComponent& component)
{
	Animator2D& a2d = *(Animator2D*)&component;
	m_Animations = a2d.m_Animations;
	m_Speed = a2d.m_Speed;
	m_CurrentAnimation = a2d.m_CurrentAnimation;
	m_Type = component.GetType();
}

IComponent* Animator2D::CreateCopy(GameObject* newOwner)
{
    Animator2D* animator = new Animator2D();
    *animator = *this;
    return animator;
}

void Animator2D::PlayAnimation(Animation2DManager::Animation2D* animation)
{
	m_Timer += Time::GetDeltaTime();
	m_Counter = m_Timer * m_Speed;
	if (m_Counter < m_CurrentAnimation->m_Textures.size())
	{
		if (Renderer2D* r2d = m_Owner->m_ComponentManager.GetComponent<Renderer2D>())
		{
			r2d->SetTexture(m_CurrentAnimation->m_Textures[m_Counter]);
		}
	}
	if (m_Counter >= m_CurrentAnimation->m_Textures.size())
	{
		m_Timer = 0;
	}
}

void Animator2D::AddAnimation(Animation2DManager::Animation2D* animation)
{
    m_Animations.push_back(animation);
}

void Animator2D::RemoveAnimation(Animation2DManager::Animation2D* animation)
{
	Utils::Erase<Animation2DManager::Animation2D>(m_Animations, animation);
}

void Animator2D::RemoveAll()
{
	m_Animations.clear();
}

void Animator2D::operator=(const Animator2D& animator2d)
{
    Copy(animator2d);
}
