#include "iw/input/Platform/Windows/WindowKeyboard.h"

#ifdef IW_PLATFORM_WINDOWS
#include <Windows.h>

namespace iw {
namespace Input {
	Keyboard* Keyboard::Create() {
		return new WindowsKeyboard();
	}

	DeviceInput WindowsKeyboard::TranslateOsEvent(
		const OsEvent& e)
	{
		return DeviceInput(DeviceType::KEYBOARD);
	}

	bool Keyboard::KeyDown(
		InputName key)
	{
		SHORT state = GetKeyState(Translate(key));
		bool  down  = state & 0x8000;
		//bool  toggled = state & 0x0008;

		return down;
	}

	bool Keyboard::KeyUp(
		InputName key)
	{
		return !KeyDown(key);
	}
}
#endif
}
