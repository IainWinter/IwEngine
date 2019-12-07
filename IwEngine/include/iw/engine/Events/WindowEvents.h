#pragma once

#include "iw/common/Events/EventGroups.h"
#include "iw/events/event.h"

namespace IW {
namespace Engine {
	enum WindowEventType {
		Closed, Destroyed, Resized, Moved
	};

	struct WindowEvent
		: iw::event
	{
		unsigned int WindowId;

		WindowEvent(
			WindowEventType type,
			unsigned int windowId)
			: iw::event(iw::val(EventGroup::WINDOW), iw::val(type))
			, WindowId(windowId)
		{}
	};

	struct WindowResizedEvent
		: WindowEvent
	{
		int Width;
		int Height;

		WindowResizedEvent(
			unsigned int windowId,
			int width,
			int height)
			: WindowEvent(Resized, windowId)
			, Width(width)
			, Height(height)
		{}
	};
}

	using namespace Engine;
}
