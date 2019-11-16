#pragma once

#include "EventGroup.h"
#include "iw/input/InputState.h"
#include "iw/events/event.h"

namespace IW {
inline namespace Engine {
	enum InputEventType {
		MouseWheel, MouseMoved, MouseButton, Key, KeyTyped
	};

	struct InputEvent
		: iw::event
	{
		IwInput::InputState* InputStates;

		InputEvent(
			InputEventType type,
			IwInput::InputState* inputState)
			: iw::event(INPUT, type)
			, InputStates(inputState)
		{}
	};

	struct MouseWheelEvent
		: InputEvent
	{
		float Delta;

		MouseWheelEvent(
			IwInput::InputState* inputState,
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
			IwInput::InputState* inputState,
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
			IwInput::InputState* inputState,
			IwInput::InputName button,
			bool state)
			: InputEvent(MouseButton, InputStates)
			, Button(button)
			, State(state)
		{}
	};

	struct KeyEvent
		: InputEvent
	{
		IwInput::InputName Button;
		bool State;

		KeyEvent(
			IwInput::InputState* inputState,
			IwInput::InputName button,
			bool state)
			: InputEvent(Key, inputState)
			, Button(button)
			, State(state)
		{}
	};

	struct KeyTypedEvent
		: InputEvent
	{
		IwInput::InputName Button;
		char Character;

		KeyTypedEvent(
			IwInput::InputState* inputState,
			IwInput::InputName button,
			char character)
			: InputEvent(KeyTyped, inputState)
			, Button(button)
			, Character(character)
		{}
	};
}
}
