#include "iw/input/Platform/Windows/WindowKeyboard.h"

#ifdef IW_PLATFORM_WINDOWS
#include <Windows.h>

namespace IW {
	Keyboard* Keyboard::Create(
		std::string name)
	{
		return new WindowsKeyboard(name);
	}

	WindowsKeyboard::WindowsKeyboard(
		std::string name)
		: Keyboard(name)
	{}

	DeviceInput WindowsKeyboard::TranslateOsEvent(
		const OsEvent& e)
	{
		return DeviceInput(KEYBOARD);
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
