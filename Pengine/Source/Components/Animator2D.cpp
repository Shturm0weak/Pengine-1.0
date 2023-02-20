#include "Animator2D.h"

#include "Renderer2D.h"
#include "../Core/GameObject.h"
#include "../EventSystem/EventSystem.h"

using namespace Pengine;


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

void Animator2D::OnRegisterClient()
{
	EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONSTART, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONCLOSE, this);
}

IComponent* Animator2D::New(GameObject* owner)
{
	return Create(owner);
}

IComponent* Animator2D::Create(GameObject* owner)
{
	return new Animator2D();
}

Animator2D::~Animator2D()
{
	EventSystem::GetInstance().UnregisterAll(this);
}

Animator2D::Animator2D(const Animator2D& a2d)
{
	Copy(a2d);
}

Animator2D& Animator2D::operator=(const Animator2D& a2d)
{
	Copy(a2d);

	return *this;
}

void Animator2D::OnUpdate()
{
	if (m_CurrentAnimation && m_Play)
	{
		PlayAnimation(m_CurrentAnimation);
	}
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
	m_FrameCounter = m_Timer * m_Speed;

	if (m_FrameCounter < m_CurrentAnimation->m_Textures.size())
	{
		Renderer2D* r2d = m_Owner->m_ComponentManager.GetComponent<Renderer2D>();

		if (r2d)
		{
			r2d->SetTexture(m_CurrentAnimation->m_Textures[m_FrameCounter]);

			if (m_AutoSetUV)
			{
				r2d->SetUV(m_CurrentAnimation->m_UVs[m_FrameCounter]);
			}
		}
	}

	if (m_FrameCounter >= m_CurrentAnimation->m_Textures.size())
	{
		m_Timer = 0.0f;

		std::unordered_map<std::string, std::function<bool()>>::iterator callbackByName = m_EndCallbacksByName.begin();
		while (callbackByName != m_EndCallbacksByName.end())
		{
			if (callbackByName->second)
			{
				if (callbackByName->second())
				{
					callbackByName = m_EndCallbacksByName.erase(callbackByName);
				}
				else
				{
					++callbackByName;
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
	Utils::Erase<Animation2DManager::Animation2D*>(m_Animations, animation);
}

void Animator2D::RemoveAllAnimations()
{
	m_Animations.clear();
}

void Animator2D::AddEndCallback(const std::string& name, std::function<bool()> callback)
{
	m_EndCallbacksByName.emplace(name, callback);
}

std::function<bool()> Animator2D::GetEndCallback(const std::string& name)
{
	auto endcallbackByName = m_EndCallbacksByName.find(name);
	if (endcallbackByName != m_EndCallbacksByName.end())
	{
		return endcallbackByName->second;
	}
	else
	{
		return {};
	}
}

void Animator2D::RemoveEndCallback(const std::string& name)
{
	auto endcallbackByName = m_EndCallbacksByName.find(name);
	if (endcallbackByName != m_EndCallbacksByName.end())
	{
		m_EndCallbacksByName.erase(name);
	}
}

void Animator2D::RemoveAllEndCallbacks()
{
	m_EndCallbacksByName.clear();
}

void Animator2D::ResetTime()
{
	m_Timer = 0.0f;
	m_FrameCounter = 0.0f;
}

std::vector<float> Animator2D::GetOriginalUV()
{
	if (m_CurrentAnimation && m_FrameCounter < m_CurrentAnimation->m_UVs.size())
	{
		return m_CurrentAnimation->m_UVs[m_FrameCounter];
	}
	else
	{
		return std::vector<float>();
	}
}

std::vector<float> Animator2D::GetReversedUV()
{
	if (m_CurrentAnimation && m_FrameCounter < m_CurrentAnimation->m_UVs.size())
	{
		std::vector<float> uv = m_CurrentAnimation->m_UVs[m_FrameCounter];
		uv[0] = m_CurrentAnimation->m_UVs[m_FrameCounter][4];
		uv[2] = m_CurrentAnimation->m_UVs[m_FrameCounter][6];
		uv[4] = m_CurrentAnimation->m_UVs[m_FrameCounter][0];
		uv[6] = m_CurrentAnimation->m_UVs[m_FrameCounter][2];

		return uv;
	}
	else
	{
		return std::vector<float>();
	}
}
