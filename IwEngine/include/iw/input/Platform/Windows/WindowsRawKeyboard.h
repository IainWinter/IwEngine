#pragma once

#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Devices/Keyboard.h"

namespace IW {
namespace Input {
	class IWINPUT_API WindowsRawKeyboard
		: public RawKeyboard
	{
	public:
		WindowsRawKeyboard();

		DeviceInput TranslateOsEvent(
			const OsEvent& e) override;
	};
}

	using namespace Input;
}
#endif
