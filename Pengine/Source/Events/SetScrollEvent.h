#pragma once

#include "../Core/Core.h"
#include "Event.h"

namespace Pengine
{

	class PENGINE_API OnSetScrollEvent : public IEvent
	{
	private:

		glm::vec2 m_Offset;
	public:

		glm::ivec2 GetOffset() const { return m_Offset; }

		OnSetScrollEvent(const glm::vec2& offset, EventType type, class IListener* sender = nullptr)
			: IEvent(type, sender)
			, m_Offset(offset)
		{

		}
	};

}