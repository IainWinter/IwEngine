#pragma once

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
}
