#pragma once

#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Devices/Keyboard.h"

namespace IW {
inline namespace Input {
	class IWINPUT_API WindowsRawKeyboard
		: public RawKeyboard
	{
	public:
		WindowsRawKeyboard(
			std::string name);

		DeviceInput TranslateOsEvent(
			const OsEvent& e) override;
	};
}
}
#endif
