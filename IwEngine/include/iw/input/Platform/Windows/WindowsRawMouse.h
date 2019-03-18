#pragma once

#include "iw/input/Devices/Mouse.h"

namespace IwInput {
	class WindowsRawMouse
		: public RawMouse
	{
	private:
		static unsigned int maskdown[5];
		static unsigned int maskup[5];

	public:
		WindowsRawMouse(
			InputCallback& callback);

		void HandleEvent(
			OsEvent& event);
	};
}
