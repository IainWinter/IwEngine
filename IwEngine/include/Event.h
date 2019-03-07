#pragma once

namespace IwEngine {
	enum EventType : unsigned int {
		WindowClosed,
		MouseMoved
	};

	struct Event {
		bool Handled;
		EventType Type;

		Event() {
			Handled = false;
		}
	};
}
