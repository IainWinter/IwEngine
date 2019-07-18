#include "iw/input/Devices/Keyboard.h"

#ifdef IW_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

namespace IwInput {
	Translation KeyboardBase::translation = CreateTranslation();

	InputName KeyboardBase::Translate(
		unsigned int oskey)
	{
		return translation.to(oskey);
	}

	unsigned int KeyboardBase::Translate(
		InputName key)
	{
		return translation.from(key);
	}

	Translation KeyboardBase::CreateTranslation() {
		Translation translation;

#ifdef IW_PLATFORM_WINDOWS
		translation.insert(VK_BACK, BACKSPACE);
		translation.insert(VK_TAB, TAB);
		translation.insert(VK_CLEAR, CLEAR);
		translation.insert(VK_RETURN, RETURN);
		translation.insert(VK_PAUSE, PAUSE);
		translation.insert(VK_ESCAPE, ESCAPE);
		translation.insert(VK_SPACE, SPACE);
		translation.insert(VK_PRIOR, PAGEUP);
		translation.insert(VK_NEXT, PADEDOWN);
		translation.insert(VK_END, END);
		translation.insert(VK_HOME, HOME);
		translation.insert(VK_SELECT, SELECT);
		translation.insert(VK_PRINT, PRINT);
		translation.insert(VK_EXECUTE, EXECUTE);
		translation.insert(VK_SNAPSHOT, PRINTSCREEN);
		translation.insert(VK_DELETE, DEL);
		translation.insert(VK_HELP, HELP);
		translation.insert('0', ZERO);
		translation.insert('1', ONE);
		translation.insert('2', TWO);
		translation.insert('3', THREE);
		translation.insert('4', FOUR);
		translation.insert('5', FIVE);
		translation.insert('6', SIX);
		translation.insert('7', SEVEN);
		translation.insert('8', EIGHT);
		translation.insert('9', NINE);
		translation.insert('A', A);
		translation.insert('B', B);
		translation.insert('C', C);
		translation.insert('D', D);
		translation.insert('E', E);
		translation.insert('F', F);
		translation.insert('G', G);
		translation.insert('H', H);
		translation.insert('I', I);
		translation.insert('J', J);
		translation.insert('K', K);
		translation.insert('L', L);
		translation.insert('M', M);
		translation.insert('N', N);
		translation.insert('O', O);
		translation.insert('P', P);
		translation.insert('Q', Q);
		translation.insert('R', R);
		translation.insert('S', S);
		translation.insert('T', T);
		translation.insert('U', U);
		translation.insert('V', V);
		translation.insert('W', W);
		translation.insert('X', X);
		translation.insert('Y', Y);
		translation.insert('Z', Z);
		translation.insert(VK_NUMPAD0, NUM0);
		translation.insert(VK_NUMPAD1, NUM1);
		translation.insert(VK_NUMPAD2, NUM2);
		translation.insert(VK_NUMPAD3, NUM3);
		translation.insert(VK_NUMPAD4, NUM4);
		translation.insert(VK_NUMPAD5, NUM5);
		translation.insert(VK_NUMPAD6, NUM6);
		translation.insert(VK_NUMPAD7, NUM7);
		translation.insert(VK_NUMPAD8, NUM8);
		translation.insert(VK_NUMPAD9, NUM9);
		translation.insert(VK_ADD, ADD);
		translation.insert(VK_SUBTRACT, SUBTRACT);
		translation.insert(VK_DIVIDE, DIVIDE);
		translation.insert(VK_MULTIPLY, MULTIPLY);
		translation.insert(VK_DECIMAL, DECIAML);
		translation.insert(VK_SEPARATOR, SEPARATOR);
		translation.insert(VK_LEFT, LEFT);
		translation.insert(VK_UP, UP);
		translation.insert(VK_RIGHT, RIGHT);
		translation.insert(VK_DOWN, DOWN);
		translation.insert(VK_F1, F1);
		translation.insert(VK_F2, F2);
		translation.insert(VK_F3, F3);
		translation.insert(VK_F4, F4);
		translation.insert(VK_F5, F5);
		translation.insert(VK_F6, F6);
		translation.insert(VK_F7, F7);
		translation.insert(VK_F8, F8);
		translation.insert(VK_F9, F9);
		translation.insert(VK_F10, F10);
		translation.insert(VK_F11, F11);
		translation.insert(VK_F12, F12);
		translation.insert(VK_F13, F13);
		translation.insert(VK_F14, F14);
		translation.insert(VK_F15, F15);
		translation.insert(VK_F16, F16);
		translation.insert(VK_F17, F17);
		translation.insert(VK_F18, F18);
		translation.insert(VK_F19, F19);
		translation.insert(VK_F20, F20);
		translation.insert(VK_F21, F21);
		translation.insert(VK_F22, F22);
		translation.insert(VK_F23, F23);
		translation.insert(VK_F24, F24);
		translation.insert(VK_CAPITAL, CAPS_LOCK);
		translation.insert(VK_NUMLOCK, NUM_LOCK);
		translation.insert(VK_SCROLL, SCROLL_LOCK);
		translation.insert(VK_SHIFT, SHIFT);
		translation.insert(VK_LSHIFT, LEFT_SHIFT);
		translation.insert(VK_RSHIFT, RIGHT_SHIFT);
		translation.insert(VK_CONTROL, CONTROL);
		translation.insert(VK_LCONTROL, LEFT_CONTROL);
		translation.insert(VK_RCONTROL, RIGHT_CONTROL);
		translation.insert(VK_MENU, MENU);
		translation.insert(VK_LMENU, LEFT_MENU);
		translation.insert(VK_RMENU, RIGHT_MENU);
		translation.insert(VK_LWIN, LEFT_WIN);
		translation.insert(VK_RWIN, RIGHT_WIN);
#endif

		return translation;
	}

}
