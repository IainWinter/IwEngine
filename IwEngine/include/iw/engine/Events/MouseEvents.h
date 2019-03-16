#pragma once

#include "Event.h"
#include "iw/input/IwInput.h"

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
		float DeltaX;
		float DeltaY;

		MouseMovedEvent(
			float x,
			float y)
			: Event(MouseMoved)
			, DeltaX(x)
			, DeltaY(y)
		{}
	};

	struct MouseButtonEvent
		: Event
	{
		IwInput::InputName Button;
		bool State;

		MouseButtonEvent(
			IwInput::InputName button,
			bool state)
			: Event(MouseButton)
			, Button(button)
			, State(state)
		{}
	};
}
