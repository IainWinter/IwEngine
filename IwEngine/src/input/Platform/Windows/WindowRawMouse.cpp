#include "iw/input/Platform/Window/WindowRawMouse.h"
#include "iw/log/logger.h"
#include <Windows.h>

namespace IwInput {
	WindowRawMouse::WindowRawMouse() {
		m_translation.emplace(WM_LBUTTONDOWN, MOUSE_L_BUTTON);
		m_translation.emplace(WM_RBUTTONDOWN, MOUSE_R_BUTTON);

		RAWINPUTDEVICE rid[1];
		rid[0].usUsagePage = 1;	  // Generic input device
		rid[0].usUsage = 2;		  // Mouse
		rid[0].dwFlags = 0;		  // No special flags
		rid[0].hwndTarget = NULL; // For current window // Seems risky

		if (!RegisterRawInputDevices(rid, 1, sizeof(rid[0]))) {
			LOG_WARNING << "Mouse failed to be created -- RAWINPUTDEVICE " << rid;
		}
	}

	void WindowRawMouse::HandleEvent(
		OsEvent& event)
	{
		//If not raw input
		if (event.Message != WM_INPUT) {
			return;
		}

		HRAWINPUT hri = (HRAWINPUT)event.LParam;
		unsigned int rihSize = sizeof(RAWINPUTHEADER);
		unsigned int dwSize = 40;
		static BYTE lpb[40];

		if (GetRawInputData(hri, RID_INPUT, lpb, &dwSize, rihSize) != dwSize) {
			LOG_WARNING << "GetRawInputData does not return correct size! HRAWINPUT " << hri;
		}

		RAWINPUT* raw = (RAWINPUT*)lpb;
		if (raw->header.dwType == RIM_TYPEMOUSE) {
			long speedX = raw->data.mouse.lLastX;
			long speedY = raw->data.mouse.lLastY;

			InputEvent x;
			x.DeviceType = MOUSE;
			x.Name       = MOUSE_X_AXIS;
			x.StateFloat = speedX / Width();

			InputEvent y;
			y.DeviceType = MOUSE;
			y.Name = MOUSE_Y_AXIS;
			y.StateFloat = speedY / Height();

			Callback(x);
			Callback(y);
		}

		//if (button) {
		//	mouse_input mouse_button = m_translation[button];
		//	handle_state_change(id(), mouse_button, down);
		//} else {
		//	float pos_x = p.x / m_width;
		//	float pos_y = p.y / m_height;
		//	float speed_x = pos_x - m_last_pos_x;
		//	float speed_y = pos_y - m_last_pos_y;

		//	m_last_pos_x = pos_x;
		//	m_last_pos_y = pos_y;

		//	handle_state_change(id(), X_POSITION, pos_x);
		//	handle_state_change(id(), Y_POSITION, pos_y);
		//	handle_state_change(id(), X_SPEED, speed_x);
		//	handle_state_change(id(), Y_SPEED, speed_y);
		//}
	}
}
