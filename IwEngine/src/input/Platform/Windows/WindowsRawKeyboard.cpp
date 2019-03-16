#include "iw/input/Platform/Windows/WindowsRawKeyboard.h"
#include "iw/log/logger.h"
#include <Windows.h>

namespace IwInput {
	Keyboard* Keyboard::Create(
		InputCallback& callback)
	{
		return new WindowsRawKeyboard(callback);
	}

	InputName Keyboard::Translate(
		unsigned int oskey)
	{
		static Translation translation = Keyboard::CreateTranslation();
		return translation[oskey];
	}

	Translation Keyboard::CreateTranslation() {
		Translation translation;
		translation.emplace(VK_BACK, BACKSPACE);
		translation.emplace(VK_TAB, TAB);
		translation.emplace(VK_CLEAR, CLEAR);
		translation.emplace(VK_RETURN, RETURN);
		translation.emplace(VK_PAUSE, PAUSE);
		translation.emplace(VK_ESCAPE, ESCAPE);
		translation.emplace(VK_SPACE, SPACE);
		translation.emplace(VK_PRIOR, PAGEUP);
		translation.emplace(VK_NEXT, PADEDOWN);
		translation.emplace(VK_END, END);
		translation.emplace(VK_HOME, HOME);
		translation.emplace(VK_SELECT, SELECT);
		translation.emplace(VK_PRINT, PRINT);
		translation.emplace(VK_EXECUTE, EXECUTE);
		translation.emplace(VK_SNAPSHOT, PRINTSCREEN);
		translation.emplace(VK_DELETE, DEL);
		translation.emplace(VK_HELP, HELP);
		translation.emplace('0', ZERO);
		translation.emplace('1', ONE);
		translation.emplace('2', TWO);
		translation.emplace('3', THREE);
		translation.emplace('4', FOUR);
		translation.emplace('5', FIVE);
		translation.emplace('6', SIX);
		translation.emplace('7', SEVEN);
		translation.emplace('8', EIGHT);
		translation.emplace('9', NINE);
		translation.emplace('A', A);
		translation.emplace('B', B);
		translation.emplace('C', C);
		translation.emplace('D', D);
		translation.emplace('E', E);
		translation.emplace('F', F);
		translation.emplace('G', G);
		translation.emplace('H', H);
		translation.emplace('I', I);
		translation.emplace('J', J);
		translation.emplace('K', K);
		translation.emplace('L', L);
		translation.emplace('M', M);
		translation.emplace('N', N);
		translation.emplace('O', O);
		translation.emplace('P', P);
		translation.emplace('Q', Q);
		translation.emplace('R', R);
		translation.emplace('S', S);
		translation.emplace('T', T);
		translation.emplace('U', U);
		translation.emplace('V', V);
		translation.emplace('W', W);
		translation.emplace('X', X);
		translation.emplace('Y', Y);
		translation.emplace('Z', Z);
		translation.emplace(VK_NUMPAD0, NUM0);
		translation.emplace(VK_NUMPAD1, NUM1);
		translation.emplace(VK_NUMPAD2, NUM2);
		translation.emplace(VK_NUMPAD3, NUM3);
		translation.emplace(VK_NUMPAD4, NUM4);
		translation.emplace(VK_NUMPAD5, NUM5);
		translation.emplace(VK_NUMPAD6, NUM6);
		translation.emplace(VK_NUMPAD7, NUM7);
		translation.emplace(VK_NUMPAD8, NUM8);
		translation.emplace(VK_NUMPAD9, NUM9);
		translation.emplace(VK_ADD, ADD);
		translation.emplace(VK_SUBTRACT, SUBTRACT);
		translation.emplace(VK_DIVIDE, DIVIDE);
		translation.emplace(VK_MULTIPLY, MULTIPLY);
		translation.emplace(VK_DECIMAL, DECIAML);
		translation.emplace(VK_SEPARATOR, SEPARATOR);
		translation.emplace(VK_LEFT, LEFT);
		translation.emplace(VK_UP, UP);
		translation.emplace(VK_RIGHT, RIGHT);
		translation.emplace(VK_DOWN, DOWN);
		translation.emplace(VK_F1, F1);
		translation.emplace(VK_F2, F2);
		translation.emplace(VK_F3, F3);
		translation.emplace(VK_F4, F4);
		translation.emplace(VK_F5, F5);
		translation.emplace(VK_F6, F6);
		translation.emplace(VK_F7, F7);
		translation.emplace(VK_F8, F8);
		translation.emplace(VK_F9, F9);
		translation.emplace(VK_F10, F10);
		translation.emplace(VK_F11, F11);
		translation.emplace(VK_F12, F12);
		translation.emplace(VK_F13, F13);
		translation.emplace(VK_F14, F14);
		translation.emplace(VK_F15, F15);
		translation.emplace(VK_F16, F16);
		translation.emplace(VK_F17, F17);
		translation.emplace(VK_F18, F18);
		translation.emplace(VK_F19, F19);
		translation.emplace(VK_F20, F20);
		translation.emplace(VK_F21, F21);
		translation.emplace(VK_F22, F22);
		translation.emplace(VK_F23, F23);
		translation.emplace(VK_F24, F24);
		translation.emplace(VK_CAPITAL, CAPS_LOCK);
		translation.emplace(VK_NUMLOCK, NUM_LOCK);
		translation.emplace(VK_SCROLL, SCROLL_LOCK);
		translation.emplace(VK_SHIFT, SHIFT);
		translation.emplace(VK_LSHIFT, LEFT_SHIFT);
		translation.emplace(VK_RSHIFT, RIGHT_SHIFT);
		translation.emplace(VK_CONTROL, CONTROL);
		translation.emplace(VK_LCONTROL, LEFT_CONTROL);
		translation.emplace(VK_RCONTROL, RIGHT_CONTROL);
		translation.emplace(VK_MENU, MENU);
		translation.emplace(VK_LMENU, LEFT_MENU);
		translation.emplace(VK_RMENU, RIGHT_MENU);
		translation.emplace(VK_LWIN, LEFT_WIN);
		translation.emplace(VK_RWIN, RIGHT_WIN);

		return translation;
	}

	WindowsRawKeyboard::WindowsRawKeyboard(
		InputCallback& callback)
		: Keyboard(callback)
	{
		RAWINPUTDEVICE rid[1];
		rid[0].usUsagePage = 1;	  // Generic input device
		rid[0].usUsage = 6;		  // Keyboard
		rid[0].dwFlags = 0;		  // No special flags
		rid[0].hwndTarget = NULL; // For current window // Seems risky

		if (!RegisterRawInputDevices(rid, 1, sizeof(rid[0]))) {
			LOG_WARNING << "Mouse failed to be created -- RAWINPUTDEVICE "
				<< rid;
		}
	}

	void WindowsRawKeyboard::HandleEvent(
		OsEvent & event)
	{
		if (event.Message != WM_INPUT) {
			return;
		}

		RAWINPUT* raw = (RAWINPUT*)event.LParam;
		if (raw->header.dwType == RIM_TYPEKEYBOARD) {
			RAWKEYBOARD keyboard = raw->data.keyboard;

			InputEvent event(KEYBOARD, event.WindowId);
			event.Name  = Translate(keyboard.VKey);
			event.State = keyboard.Message == WM_KEYDOWN;

			Callback(event);
		}
	}
}
