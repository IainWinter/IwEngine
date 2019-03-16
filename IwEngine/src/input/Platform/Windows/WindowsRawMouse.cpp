#include "iw/input/Platform/Window/WindowsRawMouse.h"
#include "iw/log/logger.h"
#include <Windows.h>

namespace IwInput {
	unsigned int WindowsRawMouse::maskdown[5] = {
		RI_MOUSE_LEFT_BUTTON_DOWN,
		RI_MOUSE_RIGHT_BUTTON_DOWN,
		RI_MOUSE_MIDDLE_BUTTON_DOWN,
		RI_MOUSE_BUTTON_4_DOWN,
		RI_MOUSE_BUTTON_5_DOWN };

	unsigned int WindowsRawMouse::maskup[5] = {
		RI_MOUSE_LEFT_BUTTON_UP,
		RI_MOUSE_RIGHT_BUTTON_UP,
		RI_MOUSE_MIDDLE_BUTTON_UP,
		RI_MOUSE_BUTTON_4_UP,
		RI_MOUSE_BUTTON_5_UP };

	Mouse* Mouse::Create(
		InputCallback& callback) {
		return new WindowsRawMouse(callback);
	}

	InputName Mouse::Translate(
		unsigned int oskey)
	{
		static Translation translation = Mouse::CreateTranslation();
		return translation.at(oskey);
	}

	Translation Mouse::CreateTranslation() {
		Translation translation;
		return translation;
	}

	WindowsRawMouse::WindowsRawMouse(
		InputCallback& callback) 
		: Mouse(callback)
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

			InputEvent event(MOUSE, event.WindowId);
			event.Name = MOUSE_L_BUTTON;
			for (int i = 0; i < 5; i++) {
				if (maskdown[i] & mouse.usButtonFlags) {
					event.State = 1;
					Callback(event);
				}

				else if (maskup[i] & mouse.usButtonFlags) {
					event.State = 0;
					Callback(event);
				}

				event.Name = (InputName)(event.Name + 1);
			}

			if (mouse.usButtonFlags == RI_MOUSE_WHEEL) {
				event.Name  = MOUSE_WHEEL;
				event.State = (short)LOWORD(mouse.usButtonData) / (float)WHEEL_DELTA;
				Callback(event);
			}

			if (mouse.usFlags == MOUSE_MOVE_RELATIVE) {
				event.Name  = MOUSE_X_AXIS;
				event.State = (float)mouse.lLastX;
				if (event.State != 0) {
					Callback(event);
				}

				event.Name  = MOUSE_Y_AXIS;
				event.State = (float)mouse.lLastY;
				if (event.State != 0) {
					Callback(event);
				}
			}
		}
	}
}
