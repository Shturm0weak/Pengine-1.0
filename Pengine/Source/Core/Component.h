#pragma once

#include "Core.h"
#include "UUID.h"

namespace Pengine
{

	class PENGINE_API IComponent
	{
	protected:

		std::string m_Type;
		class GameObject* m_Owner = nullptr;
		UUID m_UUID = UUID();

		friend class ComponentManager;
	public:

		virtual void Delete() { delete this; }
		virtual void Copy(const IComponent& component) {}
		virtual IComponent* CreateCopy(GameObject* newOwner) = 0;

		IComponent() = default;
		virtual ~IComponent() = default;

		UUID GetUUID() const { return m_UUID; }
		std::string GetType() const { return m_Type; }
		class GameObject* GetOwner() const { return m_Owner; }
	};

}
