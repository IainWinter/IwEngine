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
		float X;
		float Y;
		float DeltaX;
		float DeltaY;

		MouseMovedEvent(
			float x,
			float y,
			float deltaX,
			float deltaY)
			: Event(MouseMoved)
			, X(x)
			, Y(y)
			, DeltaX(deltaX)
			, DeltaY(deltaY)
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
