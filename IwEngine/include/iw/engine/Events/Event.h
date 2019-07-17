#pragma once

#include "iw/input/InputState.h"

namespace IwEngine {
	enum EventType : unsigned int {
		WindowClosed, WindowDestroyed, WindowResized,
		MouseWheel, MouseMoved, MouseButton,
		Key, KeyTyped,
		NOT_HANDLED
	};

	struct Event {
		EventType Type;
		bool Handled;

		Event(
			EventType type)
			: Type(type)
			, Handled(false)
		{}
	};

	struct InputEvent
		: Event
	{
		IwInput::InputState InputStates;

		InputEvent(
			EventType type,
			IwInput::InputState inputState)
			: Event(type)
			, InputStates(inputState)
		{}
	};

	struct WindowResizedEvent
		: Event
	{
		int Width;
		int Height;

		WindowResizedEvent(
			int width,
			int height)
			: Event(WindowResized)
			, Width(width)
			, Height(height)
		{}
	};
}
