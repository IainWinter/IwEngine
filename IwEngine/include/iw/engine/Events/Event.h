#pragma once

namespace IwEngine {
	enum EventType : unsigned int {
		WindowClosed, WindowDestroyed, WindowResized,
		MouseWheel, MouseMoved, MouseButton,
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
