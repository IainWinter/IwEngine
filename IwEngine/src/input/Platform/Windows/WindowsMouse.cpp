#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Platform/Windows/WindowsMouse.h"
#include <Windows.h>

#include "iw/log/logger.h"

namespace IwInput {
	Mouse* Mouse::Create(
		InputCallback& callback)
	{
		return new WindowsMouse(callback);
	}

	bool Mouse::ButtonDown(
		InputName button)
	{
		SHORT state = GetKeyState(Translate(button));
		bool  down = state & 0x8000;
		//bool  toggled = state & 0x0008;

		return down;
	}

	bool Mouse::ButtonUp(
		InputName button)
	{
		return !ButtonDown(button);
	}

	WindowsMouse::WindowsMouse(
		InputCallback& callback) 
		: Mouse(callback)
	{}

	void WindowsMouse::HandleEvent(
		OsEvent& event)
	{
		InputEvent input(MOUSE, event.WindowId);
		switch (event.Message) {
		case WM_LBUTTONDOWN:
			input.Name  = MOUSE_L_BUTTON;
			input.State = true;
			break;
		case WM_RBUTTONDOWN:
			input.Name  = MOUSE_R_BUTTON;
			input.State = true;
			break;
		case WM_MBUTTONDOWN:
			input.Name  = MOUSE_M_BUTTON;
			input.State = true;
			break;
		case WM_XBUTTONDOWN:
			input.Name  = (InputName)(MOUSE_X1_BUTTON + HIWORD(event.WParam) - 1);
			input.State = true;
			break;
		case WM_LBUTTONUP:
			input.Name  = MOUSE_L_BUTTON;
			input.State = false;
			break;
		case WM_RBUTTONUP:
			input.Name  = MOUSE_R_BUTTON;
			input.State = false;
			break;
		case WM_MBUTTONUP:
			input.Name  = MOUSE_M_BUTTON;
			input.State = false;
			break;
		case WM_XBUTTONUP:
			input.Name  = (InputName)(MOUSE_X1_BUTTON + HIWORD(event.WParam) - 1);
			input.State = false;
			break;
		}

		if (input.Name != INPUT_NONE) {
			Callback(input);
		}

		if (event.Message == WM_MOUSEWHEEL) {
			input.Name  = MOUSE_WHEEL;
			input.State = (short)HIWORD(event.WParam) / (float)WHEEL_DELTA;

			Callback(input);
		}

		if (   input.Name != INPUT_NONE 
			|| event.Message == WM_MOUSEMOVE
			|| event.Message == WM_MOUSEWHEEL)
		{
			input.Name  = MOUSE_X_POS;
			input.State = LOWORD(event.LParam);
			Callback(input);

			input.Name  = MOUSE_Y_POS;
			input.State = HIWORD(event.LParam);
			Callback(input);
		}
	}
}
#endif
