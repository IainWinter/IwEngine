#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Platform/Windows/WindowsMouse.h"

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

	vec2 Mouse::ScreenPos()
	{
		POINT p; GetCursorPos(&p);
		return vec2(float(p.x), float(p.y));
	}

	vec2 Mouse::ClientPos()
	{
		POINT p; GetCursorPos(&p); ScreenToClient(WindowFromPoint(p), &p); // should return 0 if window isnt engine
		return vec2(float(p.x), float(p.y));
	}

	DeviceInput WindowsMouse::TranslateOsEvent(
		const OsEvent& e)
	{
		DeviceInput input(DeviceType::MOUSE);
		switch (e.Message)
		{
			case WM_LBUTTONDOWN:
				input.Name  = InputName::LMOUSE;
				input.State = true;
				break;
			case WM_RBUTTONDOWN:
				input.Name  = InputName::RMOUSE;
				input.State = true;
				break;
			case WM_MBUTTONDOWN:
				input.Name  = InputName::MMOUSE;
				input.State = true;
				break;
			case WM_XBUTTONDOWN:
				input.Name  = (InputName)((int)InputName::XMOUSE1 + HIWORD(e.WParam) - 1);
				input.State = true;
				break;
			case WM_LBUTTONUP:
				input.Name  = InputName::LMOUSE;
				input.State = false;
				break;
			case WM_RBUTTONUP:
				input.Name  = InputName::RMOUSE;
				input.State = false;
				break;
			case WM_MBUTTONUP:
				input.Name  = InputName::MMOUSE;
				input.State = false;
				break;
			case WM_XBUTTONUP:
				input.Name  = (InputName)((int)InputName::XMOUSE1 + HIWORD(e.WParam) - 1);
				input.State = false;
				break;
		}

		if (input.Name != InputName::INPUT_NONE)
		{
			return input;
		}

		if (e.Message == WM_MOUSEWHEEL)
		{
			input.Name  = InputName::WHEEL;
			input.State = (short)HIWORD(e.WParam) / (float)WHEEL_DELTA;
			return input;
		}

		if (   input.Name != InputName::INPUT_NONE
			|| e.Message == WM_MOUSEMOVE
			|| e.Message == WM_MOUSEWHEEL)
		{
			POINT p;
			p.x = LOWORD(e.LParam);
			p.y = HIWORD(e.LParam);

			//ScreenToClient(e.Handle, &p);

			input.Name  = InputName::MOUSEX;
			input.State = float(p.x);

			input.Name2  = InputName::MOUSEY;
			input.State2 = float(p.y);

			return input;
		}

		return input;
	}
}
}
#endif
