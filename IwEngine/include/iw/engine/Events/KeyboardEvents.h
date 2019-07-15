#pragma once

#include "Event.h"
#include "iw/input/IwInput.h"

namespace IwEngine {
	struct KeyEvent
		: InputEvent
	{
		IwInput::InputName Button;
		bool State;

		KeyEvent(
			IwInput::InputState inputState,
			IwInput::InputName button,
			bool state)
			: InputEvent(Key, inputState)
			, Button(button)
			, State(state)
		{}
	};

	struct KeyTypedEvent
		: InputEvent
	{
		IwInput::InputName Button;
		char Character;

		KeyTypedEvent(
			IwInput::InputState inputState,
			IwInput::InputName button,
			char character)
			: InputEvent(Key, inputState)
			, Button(button)
			, Character(character)
		{}
	};
}
