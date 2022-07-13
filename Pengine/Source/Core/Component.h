#pragma once

#include "Core.h"
#include "ReflectionSystem.h"

namespace Pengine
{

	class PENGINE_API IComponent
	{
		RTTR_ENABLE()

	protected:

		size_t m_CheckTypeID = 0; // Used to see whether a void ptr from lua is castable.
		std::string m_Type;
		class GameObject* m_Owner = nullptr;

		virtual IComponent* New(GameObject* owner) = 0;
		
		friend class ComponentManager;
	public:

		IComponent() = default;
		virtual ~IComponent() = default;
		virtual void operator=(const IComponent& component) { Copy(component); }
		virtual void operator=(IComponent* component) { Copy(*component); }
		
		static bool IsValid(IComponent* component) { return component && component->m_CheckTypeID == 0; }

		virtual void Copy(const IComponent& component) {}
		
		virtual void Delete() { delete this; }
		
		virtual IComponent* CreateCopy(GameObject* newOwner);

		std::string GetType() const { return m_Type; }
		
		class GameObject* GetOwner() const { return m_Owner; }
	};

}
