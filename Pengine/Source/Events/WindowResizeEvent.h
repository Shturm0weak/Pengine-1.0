#pragma once

#include "../Core/Core.h"
#include "Event.h"

namespace Pengine
{

	class PENGINE_API WindowResizeEvent : public IEvent
	{
	private:

		glm::ivec2 m_Size;
	public:

		glm::ivec2 GetSize() const { return m_Size; }

		WindowResizeEvent(glm::ivec2 size, EventType type, class IListener* sender = nullptr) 
			: IEvent(type, sender)
			, m_Size(size)
		{

		}
	};

}