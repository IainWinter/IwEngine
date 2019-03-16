#include "iw/input/Platform/Windows/WindowsMouse.h"
#include <Windows.h>

namespace IwInput {
	void WindowsMouse::HandleEvent(
		OsEvent& event)
	{
		POINT p;
		int button = 0;
		bool down = false;

		switch (event.Message) {
		case WM_MOUSEMOVE:
			p = {
				event.LParam & 0xffff,
				(event.LParam >> 16) & 0xffff
			};
			break;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			button = event.Message;
			down = true;
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			button = event.Message;
			down = false;
			break;
		default:
			return;
		}

		if (button) {
			InputName mouse_button = translation[button];
			Callback(mouse_button, down);
		}
		else {
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
}
