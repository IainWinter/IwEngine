#include "impl_mouse.h"

#if defined IW_PLATFORM_WINDOWS && !defined IWI_WINRAW
namespace iwinput {
	impl_mouse::impl_mouse(iwinput_state_event_bus& event_bus, float width, float height)
		: impl_input_device(event_bus), m_width(width), m_height(height), m_last_pos_x(0), m_last_pos_y(0) 
	{
		m_translation[WM_LBUTTONDOWN] = L_MOUSE;
		m_translation[WM_RBUTTONDOWN] = R_MOUSE;
	}

	void impl_mouse::process(const os_event& args) {
		POINT p;
		uint button = 0;
		bool down = false;

		switch (args.code) {
		case WM_MOUSEMOVE:
			p = { 
				args.lparam & 0xffff, 
				(args.lparam >> 16) & 0xffff
			};
			break;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			button = args.code;
			down = true;
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			button = args.code;
			down = false;
			break;
		default:
			return;
		}

		if (button) {
			mouse_input mouse_button = m_translation[button];
			handle_state_change(id(), mouse_button, down);
		} else {
			float pos_x = p.x / m_width;
			float pos_y = p.y / m_height;
			float speed_x = pos_x - m_last_pos_x;
			float speed_y = pos_y - m_last_pos_y;

			m_last_pos_x = pos_x;
			m_last_pos_y = pos_y;

			handle_state_change(id(), X_POSITION, pos_x);
			handle_state_change(id(), Y_POSITION, pos_y);
			handle_state_change(id(), X_SPEED, speed_x);
			handle_state_change(id(), Y_SPEED, speed_y);
		}
	}

	void impl_mouse::process(const update_event& args) {
		handle_state_change(id(), X_SPEED, 0);
		handle_state_change(id(), Y_SPEED, 0);
	}

	void impl_mouse::process(const settings_change_event& args) {}
}
#endif