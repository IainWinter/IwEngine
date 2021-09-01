#pragma once

#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Devices/Mouse.h"

namespace iw {
namespace Input {
	struct WindowsMouse : Mouse
	{
		IWINPUT_API
		DeviceInput TranslateOsEvent(
			const OsEvent& e) override;
	};
}

	using namespace Input;
}
#endif
