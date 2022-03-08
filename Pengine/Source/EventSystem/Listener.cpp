#include "Listener.h"

#include "../Events/Event.h"

using namespace Pengine;

void IListener::HandleEvent(IEvent* event)
{
	EventType type = event->GetType();
	switch (type)
	{
	case EventType::ONSTART:
	{
		OnStart();
		break;
	}
	case EventType::ONUPDATE:
	{
		OnUpdate();
		break;
	}
	case EventType::ONCLOSE:
	{
		OnClose();
		break;
	}
	case EventType::ONWINDOWRESIZE:
	{
		OnWindowResize(*(WindowResizeEvent*)event);
		break;
	}
	default:
		break;
	}
}
