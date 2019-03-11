#pragma once

#include "iw/input/input_ids.h"

namespace IwEngine {
	enum EventType : unsigned int {
		WindowClosed, WindowDestroyed, WindowResized,
		MouseMoved, MouseButtonPressed, MouseButtonReleased,
		NOT_HANDLED
	};

	struct Event {
		bool Handled;
		EventType Type;

		Event()
			: Handled(false)
			, Type(NOT_HANDLED)
		{}

		Event(
			EventType type)
			: Handled(false)
			, Type(type)
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

	struct MouseMovedEvent
		: Event
	{
		int X;
		int Y;

		MouseMovedEvent(
			int x,
			int y)
			: Event(MouseMoved)
			, X(x)
			, Y(y)
		{}
	};

	struct MouseButtonEvent
		: Event
	{
		bool State;
		iwi::mouse_input Button;

		MouseButtonEvent(
			bool state,
			iwi::mouse_input button)
			: Event(state ? MouseButtonPressed : MouseButtonReleased)
			, State(state)
			, Button(button)
		{}
	};
}
