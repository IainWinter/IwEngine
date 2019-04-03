#pragma once

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
