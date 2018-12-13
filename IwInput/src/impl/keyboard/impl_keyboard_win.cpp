#include "impl_keyboard.h"

#ifdef IW_PLATFORM_WINDOWS

using namespace iwinput;

impl_keyboard::impl_keyboard(iwinput_state_event_bus& event_bus) : impl_input_device(event_bus) {
	m_translation[VK_BACK] = BACKSPACE;
	m_translation[VK_TAB] = TAB;
	m_translation[VK_CLEAR] = CLEAR;
	m_translation[VK_RETURN] = RETURN;
	m_translation[VK_PAUSE] = PAUSE;
	m_translation[VK_ESCAPE] = ESCAPE;
	m_translation[VK_SPACE] = SPACE;
	m_translation[VK_PRIOR] = PAGEUP;
	m_translation[VK_NEXT] = PADEDOWN;
	m_translation[VK_END] = END;
	m_translation[VK_HOME] = HOME;
	m_translation[VK_SELECT] = SELECT;
	m_translation[VK_PRINT] = PRINT;
	m_translation[VK_EXECUTE] = EXECUTE;
	m_translation[VK_PRINT] = PRINTSCREEN;
	m_translation[VK_DELETE] = DEL;
	m_translation[VK_HELP] = HELP;
	m_translation['0'] = ZERO;
	m_translation['1'] = ONE;
	m_translation['2'] = TWO;
	m_translation['3'] = THREE;
	m_translation['4'] = FOUR;
	m_translation['5'] = FIVE;
	m_translation['6'] = SIX;
	m_translation['7'] = SEVEN;
	m_translation['8'] = EIGHT;
	m_translation['9'] = NINE;
	m_translation['A'] = A;
	m_translation['B'] = B;
	m_translation['C'] = C;
	m_translation['D'] = D;
	m_translation['E'] = E;
	m_translation['F'] = F;
	m_translation['G'] = G;
	m_translation['H'] = H;
	m_translation['I'] = I;
	m_translation['J'] = J;
	m_translation['K'] = K;
	m_translation['L'] = L;
	m_translation['M'] = M;
	m_translation['N'] = N;
	m_translation['O'] = O;
	m_translation['P'] = P;
	m_translation['Q'] = Q;
	m_translation['R'] = R;
	m_translation['S'] = S;
	m_translation['T'] = T;
	m_translation['U'] = U;
	m_translation['V'] = V;
	m_translation['W'] = W;
	m_translation['X'] = X;
	m_translation['Y'] = Y;
	m_translation['Z'] = Z;
	m_translation[VK_NUMPAD0] = NUM0;
	m_translation[VK_NUMPAD1] = NUM1;
	m_translation[VK_NUMPAD2] = NUM2;
	m_translation[VK_NUMPAD3] = NUM3;
	m_translation[VK_NUMPAD4] = NUM4;
	m_translation[VK_NUMPAD5] = NUM5;
	m_translation[VK_NUMPAD6] = NUM6;
	m_translation[VK_NUMPAD7] = NUM7;
	m_translation[VK_NUMPAD8] = NUM8;
	m_translation[VK_NUMPAD9] = NUM9;
	m_translation[VK_ADD] = ADD;
	m_translation[VK_SUBTRACT] = SUBTRACT;
	m_translation[VK_DIVIDE] = DIVIDE;
	m_translation[VK_MULTIPLY] = MULTIPLY;
	m_translation[VK_DECIMAL] = DECIAML;
	m_translation[VK_SEPARATOR] = SEPARATOR;
	m_translation[VK_LEFT] = LEFT;
	m_translation[VK_UP] = UP;
	m_translation[VK_RIGHT] = RIGHT;
	m_translation[VK_DOWN] = DOWN;
	m_translation[VK_F1] = F1;
	m_translation[VK_F2] = F2;
	m_translation[VK_F3] = F3;
	m_translation[VK_F4] = F4;
	m_translation[VK_F5] = F5;
	m_translation[VK_F6] = F6;
	m_translation[VK_F7] = F7;
	m_translation[VK_F8] = F8;
	m_translation[VK_F9] = F9;
	m_translation[VK_F10] = F10;
	m_translation[VK_F11] = F11;
	m_translation[VK_F12] = F12;
	m_translation[VK_F13] = F13;
	m_translation[VK_F14] = F14;
	m_translation[VK_F15] = F15;
	m_translation[VK_F16] = F16;
	m_translation[VK_F17] = F17;
	m_translation[VK_F18] = F18;
	m_translation[VK_F19] = F19;
	m_translation[VK_F20] = F20;
	m_translation[VK_F21] = F21;
	m_translation[VK_F22] = F22;
	m_translation[VK_F23] = F23;
	m_translation[VK_F24] = F24;
	m_translation[VK_CAPITAL] = CAPS_LOCK;
	m_translation[VK_NUMLOCK] = NUM_LOCK;
	m_translation[VK_SCROLL] = SCROLL_LOCK;
	m_translation[VK_SHIFT] = SHIFT;
	m_translation[VK_LSHIFT] = LEFT_SHIFT;
	m_translation[VK_RSHIFT] = RIGHT_SHIFT;
	m_translation[VK_CONTROL] = CONTROL;
	m_translation[VK_LCONTROL] = LEFT_CONTROL;
	m_translation[VK_RCONTROL] = RIGHT_CONTROL;
	m_translation[VK_MENU] = MENU;
	m_translation[VK_LMENU] = LEFT_MENU;
	m_translation[VK_RMENU] = RIGHT_MENU;
	m_translation[VK_LWIN] = LEFT_WIN;
	m_translation[VK_RWIN] = RIGHT_WIN;
}

void impl_keyboard::process(const os_event& args) {
	if (args.code == WM_CHAR) {
		//std::cout << (char)args.wparam << std::endl;
		//send a char message
	}

	bool down;
	uint key;
	switch (args.code) {
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			down = true;
			key = args.wparam;
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			down = false;
			key = args.wparam;
			break;
		default:
			return;
	}

	switch (key) {
		case VK_CONTROL: {
			bool extended = args.lparam & 0x1000000;
			key = extended ? VK_LCONTROL : VK_RCONTROL;
			break;
		}
		case VK_MENU: {
			bool extended = args.lparam & 0x1000000;
			key = extended ? VK_RMENU : VK_LMENU;
			break;
		}
		case VK_SHIFT: { //TODO: need previous state to determine which shift was pressed
			uint scancode = (args.lparam >> 16) & 0xff;
			key = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
			break;
		}
	}

	keyboard_input translated = m_translation[key];

	handle_state_change(id(), translated, down);
}
#endif /* _WIN32 */
