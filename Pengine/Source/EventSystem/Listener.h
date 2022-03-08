#pragma once

#include "../Core/Core.h"
#include "../Events/WindowResizeEvent.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API IListener
	{
	protected:

		virtual void OnStart() {};
		virtual void OnUpdate() {};
		virtual void OnClose() {};
		virtual void OnWindowResize(const WindowResizeEvent& event) {};
	public:

		std::vector<int> m_RegisteredEvents;

		void HandleEvent(class IEvent* event);

		virtual ~IListener() {}
	};

}