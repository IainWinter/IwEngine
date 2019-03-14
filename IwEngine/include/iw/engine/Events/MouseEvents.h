#pragma once

#include "Event.h"

namespace iwi {
	using mouse_input = int;
}

namespace IwEngine {
	struct MouseMovedEvent
		: Event
	{
		int X;
		int Y;

		MouseMovedEvent(
			EventType type,
			int x,
			int y)
			: Event(type)
			, X(x)
			, Y(y)
		{}
	};

	struct MouseButtonEvent
		: Event
	{
		iwi::mouse_input Button;
		int X;
		int Y;

		MouseButtonEvent(
			EventType type,
			iwi::mouse_input button,
			int x,
			int y)
			: Event(type)
			, Button(button)
			, X(x)
			, Y(y)
		{}
	};

	struct MouseButtonPressedEvent
		: MouseButtonEvent
	{
		MouseButtonPressedEvent(
			int x,
			int y,
			iwi::mouse_input button)
			: MouseButtonEvent(MouseButtonPressed, button, x, y)
		{}
	};

	struct MouseButtonReleasedEvent
		: MouseButtonEvent
	{
		MouseButtonReleasedEvent(
			int x,
			int y,
			iwi::mouse_input button)
			: MouseButtonEvent(MouseButtonReleased, button, x, y)
		{}
	};
}
