#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Platform/Windows/WindowsMouse.h"
#include <Windows.h>

#include "iw/log/logger.h"

namespace iw {
namespace Input {
	Mouse* Mouse::Create() {
		return new WindowsMouse();
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

	vector2 Mouse::ScreenPos() {
		POINT p; GetCursorPos(&p);
		return vector2(p.x, p.y);
	}

	DeviceInput WindowsMouse::TranslateOsEvent(
		const OsEvent& e)
	{
		DeviceInput input(DeviceType::MOUSE);
		switch (e.Message) {
			case WM_LBUTTONDOWN:
				input.Name  = LMOUSE;
				input.State = true;
				break;
			case WM_RBUTTONDOWN:
				input.Name  = RMOUSE;
				input.State = true;
				break;
			case WM_MBUTTONDOWN:
				input.Name  = MMOUSE;
				input.State = true;
				break;
			case WM_XBUTTONDOWN:
				input.Name  = (InputName)(XMOUSE1 + HIWORD(e.WParam) - 1);
				input.State = true;
				break;
			case WM_LBUTTONUP:
				input.Name  = LMOUSE;
				input.State = false;
				break;
			case WM_RBUTTONUP:
				input.Name  = RMOUSE;
				input.State = false;
				break;
			case WM_MBUTTONUP:
				input.Name  = MMOUSE;
				input.State = false;
				break;
			case WM_XBUTTONUP:
				input.Name  = (InputName)(XMOUSE1 + HIWORD(e.WParam) - 1);
				input.State = false;
				break;
		}

		if (input.Name != INPUT_NONE) {
			return input;
		}

		if (e.Message == WM_MOUSEWHEEL) {
			input.Name  = WHEEL;
			input.State = (short)HIWORD(e.WParam) / (float)WHEEL_DELTA;
			return input;
		}

		if (   input.Name != INPUT_NONE 
			|| e.Message == WM_MOUSEMOVE
			|| e.Message == WM_MOUSEWHEEL)
		{
			POINT p;
			p.x = LOWORD(e.LParam);
			p.y = HIWORD(e.LParam);

			//ScreenToClient(e.Handle, &p);

			input.Name  = MOUSEX;
			input.State = p.x;

			input.Name2  = MOUSEY;
			input.State2 = p.y;

			return input;
		}

		return input;
	}
}
#endif
}
