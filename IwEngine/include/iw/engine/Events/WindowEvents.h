#pragma once

#include "EventGroup.h"
#include "iw/events/event.h"

namespace IW {
inline namespace Engine {
	enum WindowEventType {
		Closed, Destroyed, Resized, Moved
	};

	struct WindowEvent
		: iw::event
	{
		int WindowId;

		WindowEvent(
			WindowEventType type,
			int windowId)
			: iw::event(WINDOW, type)
			, WindowId(windowId)
		{}
	};

	struct WindowResizedEvent
		: WindowEvent
	{
		int Width;
		int Height;

		WindowResizedEvent(
			int id, 
			int width,
			int height)
			: WindowEvent(Resized, id)
			, Width(width)
			, Height(height)
		{}
	};
}
}
