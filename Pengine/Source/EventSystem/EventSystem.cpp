#include "EventSystem.h"

#include "../Core/EntryPoint.h"
#include "../Events/OnMainThreadCallback.h"
#include "../Core/Application.h"
#include "../Core/Timer.h"

using namespace Pengine;

void EventSystem::DispatchEvent(IEvent* event)
{
	if (event->GetType() == EventType::ONMAINTHREADPROCESS)
	{
		((OnMainThreadCallback*)event)->Run();
		return;
	}
	
	if (EntryPoint::GetApplication().GetState() != Application::ApplicationState::Play
		&& (event->GetType() == EventType::ONUPDATE)) return;

	m_Range = m_Database.equal_range((EventType)event->GetType());
	for (m_Iter = m_Range.first; m_Iter != m_Range.second; m_Iter++)
	{
		(*m_Iter).second->HandleEvent(event);
	}
}

EventSystem::~EventSystem()
{
	m_Database.clear();
	ClearEvents();
}

EventSystem& EventSystem::GetInstance()
{
	static EventSystem eventSystem;
	return eventSystem;
}

bool EventSystem::AlreadySended(IEvent* event)
{
	auto currentEventIter = std::find_if(m_CurrentEvents.begin(), m_CurrentEvents.end(),
		[=](IEvent* currentEvent)
		{
			return event->GetType() == currentEvent->GetType();
		}
	);

	if (currentEventIter != m_CurrentEvents.end())
	{
		IEvent* eventToDelete = *currentEventIter;
		delete eventToDelete;
		m_CurrentEvents.erase(currentEventIter);
		m_CurrentEvents.push_back(event);
		return true;
	}
	else
	{
		return false;
	}
}

bool EventSystem::AlreadyRegistered(EventType type, IListener* client)
{
	bool alreadyRegistered = false;

	std::pair<std::multimap<EventType, IListener*>::iterator,
		std::multimap<EventType, IListener*>::iterator> range;

	range = m_Database.equal_range(type);

	for (std::multimap<EventType, IListener*>::iterator multimapIter = range.first;
		multimapIter != range.second; multimapIter++)
	{
		if ((*multimapIter).second == client)
		{
			alreadyRegistered = true;
			break;
		}
	}

	return alreadyRegistered;
}

void EventSystem::SetProcessingEventsEnabled(bool enabled)
{
	m_IsProcessingEvents = enabled;
}

void EventSystem::RegisterClient(EventType type, IListener* client)
{
	if (!client || AlreadyRegistered(type, client))
	{
		return;
	}

	m_Database.insert(std::make_pair(type, client));
}

void EventSystem::UnregisterClient(EventType type, IListener* client)
{
	std::pair<std::multimap<EventType, IListener*>::iterator,
		std::multimap<EventType, IListener*>::iterator> range;

	range = m_Database.equal_range(type);

	for (std::multimap<EventType, IListener*>::iterator multimapIter = range.first;
		multimapIter != range.second; multimapIter++)
	{
		if ((*multimapIter).second == client)
		{
			multimapIter = m_Database.erase(multimapIter);
			break;
		}
	}
}

void EventSystem::UnregisterAll(IListener* client)
{
	if (m_Database.size() > 0)
	{
		std::multimap<EventType, IListener*>::iterator multimapIter = m_Database.begin();
		while (multimapIter != m_Database.end())
		{
			if ((*multimapIter).second == client)
			{
				multimapIter = m_Database.erase(multimapIter);
			}
			else
			{
				multimapIter++;
			}
		}
	}
}

void EventSystem::SendEvent(IEvent* event)
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	assert(event);

	if (m_IsProcessingEvents == false 
		|| (event->GetType() != EventType::ONMAINTHREADPROCESS && AlreadySended(event)))
	{
		return;
	}

	m_CurrentEvents.push_back(event);
}

void EventSystem::SendCallbackOnFrame(std::function<void()> callback, size_t frames)
{
	assert(frames != 0);

	std::vector<std::function<void()>> callbacks;

	callbacks.push_back([this, callback]
		{
			Timer::SetCallback([this, callback]
				{
					SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
				}
			, 0.0f);
		}
	);

	for (size_t i = 1; i < frames; i++)
	{
		callbacks.push_back([this, lastCallback = callbacks.back()]
			{
				Timer::SetCallback([this, lastCallback]
				{
					SendEvent(new OnMainThreadCallback(lastCallback, EventType::ONMAINTHREADPROCESS));
				}
			, 0.0f);
			}
		);
	}

	callbacks.back()();
}

void EventSystem::ProcessEvents()
{
	if (m_IsProcessingEvents == false)
	{
		return;
	}

	while (m_CurrentEvents.size() > 0)
	{
		IEvent* currentEvent = m_CurrentEvents.front();
		m_CurrentEvents.pop_front();
		DispatchEvent(currentEvent);
		delete currentEvent;
	}
}

void EventSystem::ClearEvents()
{
	while (m_CurrentEvents.size() > 0)
	{
		IEvent* currentEvent = m_CurrentEvents.front();
		m_CurrentEvents.pop_front();
		delete currentEvent;
	}
}
