#pragma once

#include "iw/input/Devices/Keyboard.h"

#ifdef IW_PLATFORM_WINDOWS
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
