#pragma once

#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Devices/Keyboard.h"

namespace IW {
namespace Input {
	class IWINPUT_API WindowsKeyboard
		: public Keyboard
	{
	public:
		DeviceInput TranslateOsEvent(
			const OsEvent& e) override;
	};
}

	using namespace Input;
}
#endif
