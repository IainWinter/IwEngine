#pragma once

#include "Event.h"
#include "iw/input/input_ids.h"

namespace IwEngine {
	struct MouseMovedEvent
		: Event
	{
		int X;
		int Y;

		MouseMovedEvent(
			int x,
			int y)
			: Event(MouseMoved)
			, X(x)
			, Y(y)
		{}
	};

	struct MouseButtonPressedEvent
		: Event
	{
		iwi::mouse_input Button;

		MouseButtonPressedEvent(
			bool state,
		iwi::mouse_input button)
			: Event(MouseButtonPressed)
			, Button(button)
		{}
	};

	struct MouseButtonReleasedEvent
		: Event
	{
		iwi::mouse_input Button;

		MouseButtonReleasedEvent(
			iwi::mouse_input button)
			: Event(MouseButtonReleased)
			, Button(button)
		{}
	};
}
