#include "ComponentManager.h"

#include "GameObject.h"

using namespace Pengine;

template<>
Transform* ComponentManager::GetComponent<Transform>()
{
	if (m_Owner)
	{
		return &m_Owner->m_Transform;
	}
}

void ComponentManager::Copy(const ComponentManager& componentManager)
{
	size_t componentsSize = componentManager.m_Components.size();
	for (size_t i = 0; i < componentsSize; i++)
	{
		IComponent* component = componentManager.m_Components[i];
		AddComponent(component->CreateCopy(m_Owner));
	}
}

void ComponentManager::operator=(const ComponentManager& componentManager)
{
	Copy(componentManager);
}

IComponent* ComponentManager::GetComponent(const std::string& type)
{
	for (auto component : m_Components)
	{
		if (component->GetType() == type)
		{
			return component;
		}
	}
	return nullptr;
}

bool ComponentManager::AddComponent(IComponent* component)
{
	if (GetComponent(component->GetType()))
	{
		return false;
	}
	else
	{
		component->m_Owner = m_Owner;
		component->m_UUID = UUID::Generate();
		m_Components.push_back(component);
#ifdef _DEBUG
		Logger::Log("has been added to GameObject!",
			component->GetType().c_str(), m_Owner->GetName().c_str(), RESET);
#endif
		return true;
	}
}