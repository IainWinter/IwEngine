#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Platform/Windows/WindowsRawMouse.h"
#include "iw/log/logger.h"
#include <Windows.h>

namespace IwInput {
	unsigned int WindowsRawMouse::maskdown[5] = {
		RI_MOUSE_LEFT_BUTTON_DOWN,
		RI_MOUSE_RIGHT_BUTTON_DOWN,
		RI_MOUSE_MIDDLE_BUTTON_DOWN,
		RI_MOUSE_BUTTON_4_DOWN,
		RI_MOUSE_BUTTON_5_DOWN
	};

	unsigned int WindowsRawMouse::maskup[5] = {
		RI_MOUSE_LEFT_BUTTON_UP,
		RI_MOUSE_RIGHT_BUTTON_UP,
		RI_MOUSE_MIDDLE_BUTTON_UP,
		RI_MOUSE_BUTTON_4_UP,
		RI_MOUSE_BUTTON_5_UP 
	};

	RawMouse* RawMouse::Create(
		InputCallback& callback) 
	{
		return new WindowsRawMouse(callback);
	}

	WindowsRawMouse::WindowsRawMouse(
		InputCallback& callback) 
		: RawMouse(callback)
	{
		RAWINPUTDEVICE rid[1];
		rid[0].usUsagePage = 1;	  // Generic input device
		rid[0].usUsage = 2;		  // Mouse
		rid[0].dwFlags = 0;		  // No special flags
		rid[0].hwndTarget = NULL; // For current window // Seems risky

		if (!RegisterRawInputDevices(rid, 1, sizeof(rid[0]))) {
			LOG_WARNING << "Mouse failed to be created -- RAWINPUTDEVICE "
				<< rid;
		}
	}

	void WindowsRawMouse::HandleEvent(
		OsEvent& event)
	{
		if (event.Message != WM_INPUT) {
			return;
		}

		RAWINPUT* raw = (RAWINPUT*)event.LParam;
		if (raw->header.dwType == RIM_TYPEMOUSE) {
			RAWMOUSE mouse = raw->data.mouse;

			InputEvent input(MOUSE, event.WindowId);
			input.Name = MOUSE_L_BUTTON;
			for (int i = 0; i < 5; i++) {
				if (maskdown[i] & mouse.usButtonFlags) {
					input.State = 1;
					Callback(input);
				}

				else if (maskup[i] & mouse.usButtonFlags) {
					input.State = 0;
					Callback(input);
				}

				input.Name = (InputName)(input.Name + 1);
			}

			if (mouse.usButtonFlags == RI_MOUSE_WHEEL) {
				input.Name  = MOUSE_WHEEL;
				input.State = (short)LOWORD(mouse.usButtonData) / (float)WHEEL_DELTA;
				Callback(input);
			}

			if (mouse.usFlags == MOUSE_MOVE_RELATIVE) {
				input.Name  = MOUSE_X_AXIS;
				input.State = (float)mouse.lLastX;
				Callback(input);

				input.Name  = MOUSE_Y_AXIS;
				input.State = (float)mouse.lLastY;
				Callback(input);
			}
		}
	}
}
#endif
