#pragma once

#include "../Core/Core.h"
#include "../Core/ReflectionSystem.h"
#include "../Events/WindowResizeEvent.h"
#include "../Events/SetScrollEvent.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API IListener
	{
	protected:

		virtual void OnStart() {}
		
		virtual void OnUpdate() {}
		
		virtual void OnClose() {}
		
		virtual void OnWindowResize(const WindowResizeEvent& event) {}

		virtual void OnSetScroll(const OnSetScrollEvent& event) {}
	public:

		void HandleEvent(class IEvent* event);

		virtual ~IListener() {}
	};

}