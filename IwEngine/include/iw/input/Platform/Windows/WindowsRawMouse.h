#pragma once

#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Devices/Mouse.h"

namespace iw {
namespace Input {
	struct WindowsRawMouse : RawMouse
	{
		IWINPUT_API
		WindowsRawMouse();

		IWINPUT_API
		DeviceInput TranslateOsEvent(
			const OsEvent& e) override;
	private:
		static unsigned int maskdown[5];
		static unsigned int maskup[5];

	};
}

	using namespace Input;
}
#endif
