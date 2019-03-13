#pragma once

#include "Event.h"
#include "iw/input/input_ids.h"

namespace IwEngine {
	struct MouseMovedEvent {
		
	};

	struct MouseButtonEvent
		: Event
	{
		iwi::mouse_input Button;
		int X;
		int Y;

		MouseButtonEvent(
			EventType type,
			iwinput::mouse_input button,
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

	struct MouseButtonPressedEvent
		: MouseButtonEvent
	{
		MouseButtonPressedEvent(
			int x,
			int y,
			iwi::mouse_input button)
			: MouseButtonEvent(MouseButtonReleased, button, x, y)
		{}
	};
}
