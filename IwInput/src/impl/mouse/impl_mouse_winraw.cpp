#include "impl_mouse.h"

#ifdef IWI_WINRAW
namespace iwinput {
	impl_mouse::impl_mouse(iwinput_state_event_bus& event_bus, float width, float height)
		: impl_input_device(event_bus), m_width(width), m_height(height), m_pos_last_x(0), m_pos_last_y(0)
	{
		m_translation[WM_LBUTTONDOWN] = L_MOUSE;
		m_translation[WM_RBUTTONDOWN] = R_MOUSE;

		RAWINPUTDEVICE rid[1];
		rid[0].usUsagePage = 1;	  // Generic input device
		rid[0].usUsage = 2;		  // Mouse
		rid[0].dwFlags = 0;		  // No special flags
		rid[0].hwndTarget = NULL; // For current window

		if (!RegisterRawInputDevices(rid, 1, sizeof(rid[0]))) {
			//Set some flag to indicate a bad registration.
		}

		POINT p;
		GetCursorPos(&p);
		m_pos_last_x = p.x;
		m_pos_last_y = p.y;
	}

	void impl_mouse::process(const os_event& args) {
		//If not raw input
		if (args.code != WM_INPUT) {
			return;
		}

		HRAWINPUT hri = (HRAWINPUT)args.lparam;
		uint rih_size = sizeof(RAWINPUTHEADER);
		uint dw_size = 40;
		static BYTE lpb[40];

		if (GetRawInputData(hri, RID_INPUT, lpb, &dw_size, rih_size) != dw_size) {
			//std::cout << "GetRawInputData does not return correct size!" << std::endl;
		}

		RAWINPUT* raw = (RAWINPUT*)lpb;
		if (raw->header.dwType == RIM_TYPEMOUSE) {
			long speed_x = raw->data.mouse.lLastX;
			long speed_y = raw->data.mouse.lLastY;
			long pos_x = speed_x + m_pos_last_x;
			long pos_y = speed_y + m_pos_last_y;

			m_pos_last_x = pos_x;
			m_pos_last_y = pos_y;

			handle_state_change(id(), X_POSITION, pos_x / m_width);
			handle_state_change(id(), Y_POSITION, pos_y / m_height);
			handle_state_change(id(), X_SPEED, speed_x / m_width);
			handle_state_change(id(), Y_SPEED, speed_y / m_height);
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

	void impl_mouse::process(const update_event& args) {
		handle_state_change(id(), X_SPEED, 0);
		handle_state_change(id(), Y_SPEED, 0);
	}

	void impl_mouse::process(const settings_change_event& args) {}
}
#endif