#pragma once

#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Devices/Keyboard.h"

namespace IwInput {
	class IWINPUT_API WindowsKeyboard
		: public Keyboard
	{
	public:
		WindowsKeyboard(
			InputCallback& callback);

		void HandleEvent(
			OsEvent& event);
	};
}
#endif
