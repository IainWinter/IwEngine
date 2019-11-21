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
		IW::InputState* InputStates; // this cant be a pointer or needs a function for getting states instead of operator

		InputEvent(
			InputEventType type,
			IW::InputState* inputState)
			: iw::event(INPUT, type)
			, InputStates(inputState)
		{}
	};

	struct MouseWheelEvent
		: InputEvent
	{
		float Delta;

		MouseWheelEvent(
			IW::InputState* inputState,
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
			IW::InputState* inputState,
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
		IW::InputName Button;
		bool State;

		MouseButtonEvent(
			IW::InputState* inputState,
			IW::InputName button,
			bool state)
			: InputEvent(MouseButton, InputStates)
			, Button(button)
			, State(state)
		{}
	};

	struct KeyEvent
		: InputEvent
	{
		IW::InputName Button;
		bool State;

		KeyEvent(
			IW::InputState* inputState,
			IW::InputName button,
			bool state)
			: InputEvent(Key, inputState)
			, Button(button)
			, State(state)
		{}
	};

	struct KeyTypedEvent
		: InputEvent
	{
		IW::InputName Button;
		char Character;

		KeyTypedEvent(
			IW::InputState* inputState,
			IW::InputName button,
			char character)
			: InputEvent(KeyTyped, inputState)
			, Button(button)
			, Character(character)
		{}
	};
}
}
