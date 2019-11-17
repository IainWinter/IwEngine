#include "iw/input/Platform/Windows/WindowsRawKeyboard.h"
#include "iw/log/logger.h"

#ifdef IW_PLATFORM_WINDOWS
#include <Windows.h>

namespace IW {
	RawKeyboard* RawKeyboard::Create(
		InputCallback& callback)
	{
		return new WindowsRawKeyboard(callback);
	}

	WindowsRawKeyboard::WindowsRawKeyboard(
		InputCallback& callback)
		: RawKeyboard(callback)
	{
		RAWINPUTDEVICE rid[1];
		rid[0].usUsagePage = 1;	  // Generic input device
		rid[0].usUsage = 6;		  // Keyboard
		rid[0].dwFlags = 0;		  // No special flags
		rid[0].hwndTarget = NULL; // For current window // Seems risky

		if (!RegisterRawInputDevices(rid, 1, sizeof(rid[0]))) {
			LOG_WARNING << "Mouse failed to be created -- RAWINPUTDEVICE "
				<< rid;
		}
	}

	void WindowsRawKeyboard::HandleEvent(
		OsEvent& event)
	{
		if (event.Message != WM_INPUT) {
			return;
		}

		RAWINPUT* raw = (RAWINPUT*)event.LParam;
		if (raw->header.dwType == RIM_TYPEKEYBOARD) {
			RAWKEYBOARD keyboard = raw->data.keyboard;

			InputEvent event(KEYBOARD, event.WindowId);

			if (keyboard.VKey == VK_UP) {
				LOG_INFO << keyboard.VKey;
			}

			event.Name  = Translate(keyboard.VKey);
			event.State = !keyboard.Flags;

			Callback(event);
		}
	}
}
#endif
