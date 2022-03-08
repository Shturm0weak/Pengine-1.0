#pragma once

#include "../Core/Core.h"
#include "../Events/Event.h"
#include "Listener.h"

#include <deque>
#include <map>
#include <unordered_map>
#include <vector>

namespace Pengine
{

	class PENGINE_API EventSystem
	{
	private:

		std::multimap<EventType, IListener*> m_Database;

		std::pair<std::multimap<EventType, IListener*>::iterator,
			std::multimap<EventType, IListener*>::iterator> m_Range;

		std::multimap<EventType, IListener*>::iterator m_Iter;

		std::deque<IEvent*> m_CurrentEvents;
		bool m_IsProcessingEvents = true;
		
		std::mutex m_Mutex;

		void DispatchEvent(IEvent* event);

		EventSystem() = default;
		EventSystem(const EventSystem&) = delete;
		EventSystem& operator=(const EventSystem&) { return *this; }
		~EventSystem();
	public:

		static EventSystem& GetInstance();
		
		bool AlreadySended(IEvent* event);
		bool AlreadyRegistered(EventType type, IListener* client);
		void SetProcessingEventsEnabled(bool enabled);
		void RegisterClient(EventType type, IListener* client);
		void UnregisterClient(EventType type, IListener* client);
		void UnregisterAll(IListener* client);
		void SendEvent(IEvent* event);
		void ProcessEvents();
		void ClearEvents();
	};

}
