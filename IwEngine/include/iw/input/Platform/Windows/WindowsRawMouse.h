#pragma once

#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Devices/Mouse.h"

namespace IW {
inline namespace Input {
	class IWINPUT_API WindowsRawMouse
		: public RawMouse
	{
	private:
		static unsigned int maskdown[5];
		static unsigned int maskup[5];

	public:
		WindowsRawMouse(
			std::string name);

		InputEvent TranslateOsEvent(
			const OsEvent& event) override;
	};
}
}
#endif
