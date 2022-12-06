#pragma once

#include "Core.h"
#include "ReflectionSystem.h"

namespace Pengine
{

	class PENGINE_API IComponent
	{
		RTTR_ENABLE()

	protected:

		/**
		 * If the instance is corrupted, it may shows this.
		 */
		size_t m_CheckID = 0;

		std::string m_Type;

		class GameObject* m_Owner = nullptr;

		virtual IComponent* New(GameObject* owner) = 0;
		
		virtual void Copy(const IComponent& component) = 0;

		virtual void Move(IComponent&& component) {};

		virtual void Delete() { delete this; }

		virtual void OnRegisterClient() {}

		virtual IComponent* CreateCopy(GameObject* newOwner);

		friend class ComponentManager;
		friend class LuaState;
		friend class Scene;
	public:

		static bool IsValid(IComponent* component) { return component && component->m_CheckID == 0; }

		IComponent() = default;
		virtual ~IComponent() = default;
		virtual void operator=(const IComponent& component) { Copy(component); }
		virtual void operator=(IComponent* component) { Copy(*component); }

		std::string GetType() const { return m_Type; }
		
		class GameObject* GetOwner() const { return m_Owner; }
	};

}
