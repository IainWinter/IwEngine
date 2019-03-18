#pragma once

#include "Event.h"
#include "iw/input/IwInput.h"

namespace IwEngine {
	struct KeyEvent
		: Event
	{
		IwInput::InputName Button;
		bool State;

		KeyEvent(
			IwInput::InputName button,
			bool state)
			: Event(Key)
			, Button(button)
			, State(state)
		{}
	};
}
