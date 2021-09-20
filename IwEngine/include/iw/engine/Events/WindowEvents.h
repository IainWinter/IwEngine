#pragma once

#include "iw/events/event.h"
#include "iw/common/Events/EventGroups.h"

namespace iw {
namespace Engine {
	enum WindowEventType {
		Closed, Destroyed, Resized, Moved
	};

	struct REFLECT WindowEvent
		: event
	{
		unsigned int WindowId;

		WindowEvent(
			WindowEventType type,
			unsigned int windowId
		)
			: event(val(EventGroup::WINDOW), val(type))
			, WindowId(windowId)
		{}
	};

	struct REFLECT WindowResizedEvent
		: WindowEvent
	{
		int Width;
		int Height;

		WindowResizedEvent(
			unsigned int windowId,
			int width,
			int height
		)
			: WindowEvent(Resized, windowId)
			, Width(width)
			, Height(height)
		{}
	};
}

	using namespace Engine;
}
