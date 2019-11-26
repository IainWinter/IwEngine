#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Platform/Windows/WindowsRawMouse.h"
#include "iw/log/logger.h"
#include <Windows.h>

namespace IW {
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
		std::string name)
	{
		return new WindowsRawMouse(name);
	}

	WindowsRawMouse::WindowsRawMouse(
		std::string name)
		: RawMouse(name)
	{
		RAWINPUTDEVICE rid[1];
		rid[0].usUsagePage = 1;	  // Generic input device
		rid[0].usUsage = 2;		  // Mouse
		rid[0].dwFlags = 0;		  // No special flags
		rid[0].hwndTarget = NULL; // For current window // Seems risky

		if (!RegisterRawInputDevices(rid, 1, sizeof(rid[0]))) {
			LOG_WARNING << "Mouse failed to be created -- RAWINPUTDEVICE " << rid;
		}
	}

	DeviceInput WindowsRawMouse::TranslateOsEvent(
		const OsEvent& e)
	{
		if (e.Message != WM_INPUT) {
			return DEVICE_NONE;
		}

		RAWINPUT* raw = (RAWINPUT*)e.LParam;
		if (raw->header.dwType == RIM_TYPEMOUSE) {
			RAWMOUSE mouse = raw->data.mouse;

			DeviceInput input(MOUSE);
			input.Name = LMOUSE;
			for (int i = 0; i < 5; i++) {
				if (maskdown[i] & mouse.usButtonFlags) {
					input.State = 1;
					return input;
				}

				else if (maskup[i] & mouse.usButtonFlags) {
					input.State = 0;
					return input;
				}

				input.Name = (InputName)(input.Name + 1);
			}

			if (mouse.usButtonFlags == RI_MOUSE_WHEEL) {
				input.Name  = WHEEL;
				input.State = (short)LOWORD(mouse.usButtonData) / (float)WHEEL_DELTA;
				return input;
			}

			if (mouse.usFlags == MOUSE_MOVE_RELATIVE) {
				input.Name  = MOUSEdX;
				input.State = (float)mouse.lLastX;

				input.Name2  = MOUSEdY;
				input.State2 = (float)mouse.lLastY; // xd
				return input;
			}
		}
	}
}
#endif
