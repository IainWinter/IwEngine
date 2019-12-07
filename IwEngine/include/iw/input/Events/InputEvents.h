#pragma once

#include "iw/common/Events/EventGroups.h"
#include "iw/input/InputState.h"
#include "iw/events/event.h"

namespace IW {
namespace Engine {
	enum class InputEventType
		: short
	{
		MouseWheel, MouseMoved, MouseButton, Key, KeyTyped, Command
	};

	struct InputEvent
		: iw::event
	{
		DeviceType Device;
		InputState* InputStates; // this cant be a pointer or needs a function for getting states instead of operator

		InputEvent(
			InputEventType type,
			DeviceType device,
			InputState* inputState)
			: iw::event(iw::val(EventGroup::INPUT), iw::val(type))
			, Device(device)
			, InputStates(inputState)
		{}
	};

	struct InputCommandEvent
		: iw::event
	{
		std::string Command;

		InputCommandEvent(
			std::string command)
			: iw::event(iw::val(EventGroup::INPUT), iw::val(InputEventType::Command))
			, Command(command)
		{}
	};

	struct MouseWheelEvent
		: InputEvent
	{
		float Delta;

		MouseWheelEvent(
			DeviceType device,
			InputState* inputState,
			float delta)
			: InputEvent(InputEventType::MouseWheel, device, inputState)
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
			DeviceType device,
			InputState* inputState,
			float x,
			float y,
			float deltaX,
			float deltaY)
			: InputEvent(InputEventType::MouseMoved, device, inputState)
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
			DeviceType device,
			InputState* inputState,
			InputName button,
			bool state)
			: InputEvent(InputEventType::MouseButton, device, inputState)
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
			DeviceType device,
			InputState* inputState,
			InputName button,
			bool state)
			: InputEvent(InputEventType::Key, device, inputState)
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
			DeviceType device,
			InputState* inputState,
			InputName button,
			char character)
			: InputEvent(InputEventType::KeyTyped, device, inputState)
			, Button(button)
			, Character(character)
		{}
	};
}

	using namespace Engine;
}
