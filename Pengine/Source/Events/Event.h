#pragma once

#include "../Core/Core.h"
#include "../Enums/EventType.h"

namespace Pengine
{

	class PENGINE_API IEvent
	{
	private:

		class IListener* m_Sender = nullptr;
		EventType m_Type;
	public:

		EventType GetType() const { return m_Type; }

		IEvent(EventType type, class IListener* sender = nullptr)
			: m_Type(type)
			, m_Sender(sender)
		{

		}

		virtual ~IEvent() {}
	};

}