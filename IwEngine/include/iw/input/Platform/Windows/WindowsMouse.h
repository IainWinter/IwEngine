#pragma once

#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Devices/Mouse.h"

namespace IwInput {
	class WindowsMouse
		: public Mouse
	{
	public:
		WindowsMouse(
			InputCallback& callback);

		void HandleEvent(
			OsEvent& event);
	};
}
#endif
