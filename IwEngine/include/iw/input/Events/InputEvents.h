#pragma once

#include "iw/common/Events/EventGroups.h"
#include "iw/input/InputState.h"
#include "iw/events/event.h"

namespace IW {
inline namespace Engine {
	enum class InputEventType
		: short
	{
		MouseWheel, MouseMoved, MouseButton, Key, KeyTyped
	};

	struct InputEvent
		: iw::event
	{
		std::string Name;
		InputState* InputStates; // this cant be a pointer or needs a function for getting states instead of operator

		InputEvent(
			InputEventType type,
			std::string name,
			InputState* inputState)
			: iw::event(iw::val(EventGroup::INPUT), iw::val(type))
			, Name(name)
			, InputStates(inputState)
		{}
	};

	struct MouseWheelEvent
		: InputEvent
	{
		float Delta;

		MouseWheelEvent(
			std::string name,
			InputState* inputState,
			float delta)
			: InputEvent(InputEventType::MouseWheel, name, inputState)
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
			std::string name,
			InputState* inputState,
			float x,
			float y,
			float deltaX,
			float deltaY)
			: InputEvent(InputEventType::MouseMoved, name, inputState)
			, X(x)
			, Y(y)
			, DeltaX(deltaX)
			, DeltaY(deltaY)
		{}
	};

	struct MouseButtonEvent
		: InputEvent
	{
		InputName Button;
		bool State;

		MouseButtonEvent(
			std::string name,
			InputState* inputState,
			InputName button,
			bool state)
			: InputEvent(InputEventType::MouseButton, name, inputState)
			, Button(button)
			, State(state)
		{}
	};

	struct KeyEvent
		: InputEvent
	{
		InputName Button;
		bool State;

		KeyEvent(
			std::string name,
			InputState* inputState,
			InputName button,
			bool state)
			: InputEvent(InputEventType::Key, name, inputState)
			, Button(button)
			, State(state)
		{}
	};

	struct KeyTypedEvent
		: InputEvent
	{
		InputName Button;
		char Character;

		KeyTypedEvent(
			std::string name,
			InputState* inputState,
			InputName button,
			char character)
			: InputEvent(InputEventType::KeyTyped, name, inputState)
			, Button(button)
			, Character(character)
		{}
	};
}
}
