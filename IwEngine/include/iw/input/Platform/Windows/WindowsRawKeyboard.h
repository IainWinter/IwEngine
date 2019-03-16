#pragma once

#include "iw/input/Devices/Keyboard.h"
#include "iw/util/set/sparse_set.h"

namespace IwInput {
	class WindowsRawKeyboard
		: public Keyboard
	{
	public:
		WindowsRawKeyboard(
			InputCallback& callback);

		void HandleEvent(
			OsEvent& event);
	};
}
