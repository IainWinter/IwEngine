#pragma once

#include "iw/events/event.h"
#include "iw/common/Events/EventGroups.h"
#include "iw/input/InputState.h"
#include <string>

namespace iw {
namespace Input {
	enum class InputEventType
		: short
	{
		MouseWheel, MouseMoved, MouseButton, Key, KeyTyped, Command
	};

	struct REFLECT InputEvent
		: event
	{
		DeviceType Device;
		InputState& InputStates; // this cant be a pointer or needs a function for getting states instead of operator

		InputEvent(
			InputEventType type,
			DeviceType device,
			InputState& inputState
		)
			: event(val(EventGroup::INPUT), val(type))
			, Device(device)
			, InputStates(inputState)
		{}
	};

	struct REFLECT InputCommandEvent
		: event
	{
		std::string Command;

		InputCommandEvent(
			std::string command
		)
			: event(val(EventGroup::INPUT), val(InputEventType::Command))
			, Command(command)
		{}
	};

	struct REFLECT MouseWheelEvent
		: InputEvent
	{
		float Delta;

		MouseWheelEvent(
			DeviceType device,
			InputState& inputState,
			float delta
		)
			: InputEvent(InputEventType::MouseWheel, device, inputState)
			, Delta(delta)
		{}
	};

	struct REFLECT MouseMovedEvent
		: InputEvent
	{
		float X;
		float Y;
		float DeltaX;
		float DeltaY;

		MouseMovedEvent(
			DeviceType device,
			InputState& inputState,
			float x,
			float y,
			float deltaX,
			float deltaY
		)
			: InputEvent(InputEventType::MouseMoved, device, inputState)
			, X(x)
			, Y(y)
			, DeltaX(deltaX)
			, DeltaY(deltaY)
		{}
	};

	struct REFLECT MouseButtonEvent
		: InputEvent
	{
		InputName Button;
		bool State;

		MouseButtonEvent(
			DeviceType device,
			InputState& inputState,
			InputName button,
			bool state
		)
			: InputEvent(InputEventType::MouseButton, device, inputState)
			, Button(button)
			, State(state)
		{}
	};

	struct REFLECT KeyEvent
		: InputEvent
	{
		InputName Button;
		bool State;

		KeyEvent(
			DeviceType device,
			InputState& inputState,
			InputName button,
			bool state
		)
			: InputEvent(InputEventType::Key, device, inputState)
			, Button(button)
			, State(state)
		{}
	};

	struct REFLECT KeyTypedEvent
		: InputEvent
	{
		InputName Button;
		char Character;

		KeyTypedEvent(
			DeviceType device,
			InputState& inputState,
			InputName button,
			char character
		)
			: InputEvent(InputEventType::KeyTyped, device, inputState)
			, Button(button)
			, Character(character)
		{}
	};
}

	using namespace Input;
}
