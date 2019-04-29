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

	struct KeyTypedEvent
		: Event
	{
		IwInput::InputName Button;
		char Character;

		KeyTypedEvent(
			IwInput::InputName button,
			char character)
			: Event(Key)
			, Button(button)
			, Character(character)
		{}
	};
}
