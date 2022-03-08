#pragma once

#include "../Core/Core.h"
#include "Event.h"

#include <functional>

namespace Pengine
{

	class PENGINE_API OnMainThreadCallback : public IEvent
	{
	private:

		std::function<void()> m_Callback = nullptr;
	public:

		OnMainThreadCallback(std::function<void()> callback, EventType type, class IListener* sender = nullptr)
			: IEvent(type, sender)
			, m_Callback(callback)
		{

		}

		void Run()
		{
			if (m_Callback)
			{
				m_Callback();
			}
		}
	};

}