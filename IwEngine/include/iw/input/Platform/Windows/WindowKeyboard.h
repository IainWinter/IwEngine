#pragma once

#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Devices/Keyboard.h"

namespace iw {
namespace Input {
	struct WindowsKeyboard : Keyboard
	{
		IWINPUT_API
		DeviceInput TranslateOsEvent(
			const OsEvent& e) override;
	};
}

	using namespace Input;
}
#endif
