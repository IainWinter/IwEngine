#include "iw/input/Platform/Windows/WindowsMouse.h"
#include <Windows.h>

#include "iw/log/logger.h"

namespace IwInput {
	unsigned int WindowsMouse::buttonMask[5] = {
		MK_LBUTTON,
		MK_RBUTTON,
		MK_MBUTTON,
		MK_XBUTTON1,
		MK_XBUTTON2
	};

	Mouse* Mouse::Create(
		InputCallback& callback)
	{
		return new WindowsMouse(callback);
	}

	WindowsMouse::WindowsMouse(
		InputCallback& callback) 
		: Mouse(callback)
	{}

	void WindowsMouse::HandleEvent(
		OsEvent& event)
	{
		InputEvent input(MOUSE, event.WindowId);
		short buttons = 0;
		switch (event.Message) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			buttons = event.WParam;
			input.State = true;
			break;
		case WM_XBUTTONDOWN:
			buttons = LOWORD(event.WParam);
			input.State = true;
			break;
		case WM_LBUTTONUP:
			buttons = MK_LBUTTON | event.WParam;
			input.State = false;
			break;
		case WM_RBUTTONUP:
			buttons = MK_RBUTTON | event.WParam;
			input.State = false;
			break;
		case WM_MBUTTONUP:
			buttons = MK_MBUTTON | event.WParam;
			input.State = false;
			break;
		case WM_XBUTTONUP:
			buttons = MK_XBUTTON1 | MK_XBUTTON2 | LOWORD(event.WParam);
			input.State = false;
			break;
		}

		if (buttons) {
			input.Name = MOUSE_L_BUTTON;
			for (int i = 0; i < 5; i++) {
				if (buttonMask[i] & buttons) {
					Callback(input);
				}

				input.Name = (InputName)(input.Name + 1);
			}
		}

		if (event.Message == WM_MOUSEWHEEL) {
			input.Name = MOUSE_WHEEL;
			input.State = (short)HIWORD(event.WParam) / (float)WHEEL_DELTA;

			Callback(input);
		}

		if (   buttons 
			|| event.Message == WM_MOUSEMOVE
			|| event.Message == WM_MOUSEWHEEL)
		{
			input.Name = MOUSE_X_POS;
			input.State = LOWORD(event.LParam);
			Callback(input);

			input.Name = MOUSE_Y_POS;
			input.State = HIWORD(event.LParam);
			Callback(input);
		}
	}
}
