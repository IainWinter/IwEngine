#pragma once

#include "Event.h"

namespace iwi {
	using mouse_input = int;
}

namespace IwEngine {
	struct MouseWheelEvent
		: Event
	{
		float Delta;

		MouseWheelEvent(
			float delta)
			: Event(MouseWheel)
			, Delta(delta)
		{}
	};

	struct MouseMovedEvent
		: Event
	{
		float X;
		float Y;

		MouseMovedEvent(
			float x,
			float y)
			: Event(MouseMoved)
			, X(x)
			, Y(y)
		{}
	};

	struct MouseButtonEvent
		: Event
	{
		iwi::mouse_input Button;
		bool State;

		MouseButtonEvent(
			iwi::mouse_input button,
			bool state)
			: Event(MouseButton)
			, Button(button)
			, State(state)
		{}
	};
}
