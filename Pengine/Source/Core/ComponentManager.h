#pragma once

#include "Core.h"
#include "Utils.h"
#include "Logger.h"
#include "Component.h"
#include "UUID.h"
#include "../Components/Transform.h"
#include "../Components/ICollider2D.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API ComponentManager
	{
	private:

		std::vector<IComponent*> m_Components;
		class GameObject* m_Owner = nullptr;

		void Copy(const ComponentManager& componentManager);
	public:

		const std::vector<IComponent*> GetComponents() const { return m_Components; }

		ComponentManager(class GameObject* owner) : m_Owner(owner) {}

		void operator=(const ComponentManager& componentManager);

		void Clear()
		{
			for (auto component : m_Components)
			{
				component->Delete();
			}
			m_Components.clear();
		}

		IComponent* GetComponent(const std::string& type);

		template<class T>
		T* GetComponent()
		{
			for (auto component : m_Components)
			{
				if (component->GetType() == GetTypeName<T>())
				{
					return static_cast<T*>(component);
				}
			}
			return nullptr;
		}

		template<>
		Transform* GetComponent<Transform>();

		template<typename T>
		T* GetComponentSubClass()
		{
			for (auto component : m_Components)
			{
				if (dynamic_cast<T*>(component) != nullptr)
				{
					return static_cast<T*>(component);
				}
			}

			return nullptr;
		}

		bool AddComponent(IComponent* component);

		template<class T>
		T* AddComponent()
		{
			IComponent* component = GetComponent<T>();
			if (component == nullptr)
			{
				component = T::Create(m_Owner);
				component->m_Owner = m_Owner;
				component->m_Type = GetTypeName<T>();
				m_Components.push_back(component);
#ifdef _DEBUG
				Logger::Log("has been added to GameObject!",
					GetTypeName<T>().c_str(), m_Owner->GetName().c_str(), RESET);
#endif
				return static_cast<T*>(component);
			}
			return static_cast<T*>(component);
		}

		template<class T>
		void RemoveComponent()
		{
			RemoveComponent(GetComponent<T>());
		}

		void RemoveComponent(IComponent* component)
		{
			if (component == nullptr) return;

			Utils::Erase<IComponent*>(m_Components, component);

			component->Delete();
		}
	};

}
