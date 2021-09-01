#pragma once

#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Devices/Keyboard.h"

namespace iw {
namespace Input {
	struct WindowsRawKeyboard : RawKeyboard
	{
		IWINPUT_API
		WindowsRawKeyboard();

		IWINPUT_API
		DeviceInput TranslateOsEvent(
			const OsEvent& e) override;
	};
}

	using namespace Input;
}
#endif
