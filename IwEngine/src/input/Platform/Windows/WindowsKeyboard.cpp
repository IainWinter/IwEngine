#include "iw/input/Platform/Windows/WindowKeyboard.h"

#ifdef IW_PLATFORM_WINDOWS
#include <Windows.h>

namespace IwInput {
	Keyboard* Keyboard::Create(
		InputCallback& callback)
	{
		return new WindowsKeyboard(callback);
	}

	WindowsKeyboard::WindowsKeyboard(
		InputCallback& callback)
		: Keyboard(callback)
	{}

	void WindowsKeyboard::HandleEvent(
		OsEvent& event)
	{
		Translate(VK_SPACE);
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
