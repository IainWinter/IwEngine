#include "iw/input/Platform/Windows/WindowsRawKeyboard.h"
#include "iw/log/logger.h"

#ifdef IW_PLATFORM_WINDOWS
#include <Windows.h>

namespace IW {
	RawKeyboard* RawKeyboard::Create(
		std::string name)
	{
		return new WindowsRawKeyboard(name);
	}

	WindowsRawKeyboard::WindowsRawKeyboard(
		std::string name)
		: RawKeyboard(name)
	{
		RAWINPUTDEVICE rid[1];
		rid[0].usUsagePage = 1;	  // Generic input device
		rid[0].usUsage = 6;		  // Keyboard
		rid[0].dwFlags = 0;		  // No special flags
		rid[0].hwndTarget = NULL; // For current window // Seems risky

		if (!RegisterRawInputDevices(rid, 1, sizeof(rid[0]))) {
			LOG_WARNING << "Mouse failed to be created -- RAWINPUTDEVICE " << rid;
		}
	}

	InputEvent WindowsRawKeyboard::TranslateOsEvent(
		const OsEvent& event)
	{
		if (event.Message != WM_INPUT) {
			return;
		}

		RAWINPUT* raw = (RAWINPUT*)event.LParam;
		if (raw->header.dwType == RIM_TYPEKEYBOARD) {
			RAWKEYBOARD keyboard = raw->data.keyboard;

			if (keyboard.VKey == 255) return;

			InputEvent e(KEYBOARD, event.WindowId);
			e.Name  = Translate(keyboard.VKey);
			e.State = !(keyboard.Flags & RI_KEY_BREAK);

			// ways to see if key is toggled, should replace caps system that is in
			// context rn but that works for now just the states can be out of sync from the keyboard

			return e;
		}
	}
}
#endif
