#pragma once

namespace IwEngine {
	enum EventType : unsigned int {
		WindowClosed,
		MouseMoved,
		NOT_HANDLED
	};

	struct Event {
		bool Handled;
		EventType Type;

		Event() {
			Handled = false;
			Type = NOT_HANDLED;
		}
	};
}
