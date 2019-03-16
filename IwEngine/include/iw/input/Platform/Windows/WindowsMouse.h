#pragma once

#include "iw/input/Devices/Mouse.h"

namespace IwInput {
	class WindowsMouse
		: public Mouse
	{
	private:
		static unsigned int maskdown[5];
		static unsigned int maskup[5];
	public:
		WindowsMouse(
			InputCallback& callback);

		void HandleEvent(
			OsEvent& event);
	};
}
