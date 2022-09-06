#include "Animator2D.h"

#include "Renderer2D.h"
#include "../Core/GameObject.h"
#include "../EventSystem/EventSystem.h"

using namespace Pengine;

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
	m_Type = component.GetType();
	m_Animations = a2d.m_Animations;
	m_Speed = a2d.m_Speed;
	m_CurrentAnimation = a2d.m_CurrentAnimation;
	m_Play = a2d.m_Play;
}

IComponent* Animator2D::New(GameObject* owner)
{
	return Create(owner);
}

Animation2DManager::Animation2D* Animator2D::GetAnimation(const std::string& name)
{
	for (auto& anim : m_Animations)
	{
		if (anim->m_Name == name)
		{
			return anim;
		}
	}

	return nullptr;
}

void Animator2D::PlayAnimation(Animation2DManager::Animation2D* animation)
{
	m_Timer += Time::GetDeltaTime();
	m_Counter = m_Timer * m_Speed;

	if (m_Counter < m_CurrentAnimation->m_Textures.size())
	{
		m_Renderer2D = m_Renderer2D ? m_Renderer2D : m_Owner->m_ComponentManager.GetComponent<Renderer2D>();

		if (m_Renderer2D)
		{
			m_Renderer2D->SetTexture(m_CurrentAnimation->m_Textures[m_Counter]);

			if (m_AutoSetUV)
			{
				m_Renderer2D->SetUV(m_CurrentAnimation->m_UVs[m_Counter]);
			}
		}
	}

	if (m_Counter >= m_CurrentAnimation->m_Textures.size())
	{
		m_Timer = 0.0f;

		for (std::vector<std::function<bool()>>::iterator callbackIter = m_EndCallbacks.begin();
			callbackIter != m_EndCallbacks.end();)
		{
			if ((*callbackIter))
			{
				if ((*callbackIter)())
				{
					callbackIter = m_EndCallbacks.erase(callbackIter);
				}
				else
				{
					++callbackIter;
				}
			}
		}
	}
}

void Animator2D::AddAnimation(Animation2DManager::Animation2D* animation)
{
	if (!Utils::IsThere<Animation2DManager::Animation2D*>(m_Animations, animation))
	{
		m_Animations.push_back(animation);
	}
}

void Animator2D::RemoveAnimation(Animation2DManager::Animation2D* animation)
{
	Utils::Erase<Animation2DManager::Animation2D>(m_Animations, animation);
}

void Animator2D::RemoveAll()
{
	m_Animations.clear();
}

void Animator2D::Reset()
{
	m_Timer = 0.0f;
	m_Counter = 0.0f;
}

std::vector<float> Animator2D::GetOriginalUV()
{
	if (m_CurrentAnimation && m_Counter < m_CurrentAnimation->m_UVs.size())
	{
		return m_CurrentAnimation->m_UVs[m_Counter];
	}
	else
	{
		return std::vector<float>();
	}
}

std::vector<float> Animator2D::GetReversedUV()
{
	if (m_CurrentAnimation && m_Counter < m_CurrentAnimation->m_UVs.size())
	{
		std::vector<float> uv = m_CurrentAnimation->m_UVs[m_Counter];
		uv[0] = m_CurrentAnimation->m_UVs[m_Counter][4];
		uv[2] = m_CurrentAnimation->m_UVs[m_Counter][6];
		uv[4] = m_CurrentAnimation->m_UVs[m_Counter][0];
		uv[6] = m_CurrentAnimation->m_UVs[m_Counter][2];

		return uv;
	}
	else
	{
		return std::vector<float>();
	}
}
