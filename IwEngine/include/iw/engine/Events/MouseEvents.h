#pragma once

#include "Event.h"
#include "iw/input/IwInput.h"

namespace IwEngine {
	struct MouseWheelEvent
		: InputEvent
	{
		float Delta;

		MouseWheelEvent(
			IwInput::InputState inputState,
			float delta)
			: InputEvent(MouseWheel, inputState)
			, Delta(delta)
		{}
	};

	struct MouseMovedEvent
		: InputEvent
	{
		float X;
		float Y;
		float DeltaX;
		float DeltaY;

		MouseMovedEvent(
			IwInput::InputState inputState,
			float x,
			float y,
			float deltaX,
			float deltaY)
			: InputEvent(MouseMoved, inputState)
			, X(x)
			, Y(y)
			, DeltaX(deltaX)
			, DeltaY(deltaY)
		{}
	};

	struct MouseButtonEvent
		: InputEvent
	{
		IwInput::InputName Button;
		bool State;

		MouseButtonEvent(
			IwInput::InputState inputState,
			IwInput::InputName button,
			bool state)
			: InputEvent(MouseButton, InputStates)
			, Button(button)
			, State(state)
		{}
	};
}
